// Generate TS classes based upon the C code from the Ardwiino lib
const rp = require("request-promise");
const fs = require("fs");
const parser = require("node-c-parser");
const tsParser = require("typescript-parser");
const config_url = "https://raw.githubusercontent.com/sanjay900/Ardwiino/master/src/shared/config/";

//Process eeprom.h, and turn it into a tree
async function generateConfigTree() {
  let eeprom_h = parser.lexer.lexUnit.tokenize(await rp(`${config_url}/eeprom.h`));
  let reading = false;
  let types = {};
  let currentDefinitions = [];
  //Walk through tokens, and read in struct sections, turning them into types.
  for (let i = 0; i < eeprom_h.length; i++) {
    const token = eeprom_h[i];
    if (token.lexeme == "struct") {
      reading = true;
    }
    if (reading && token.lexeme == "ATTR_PACKED") {
      const typeName = eeprom_h[++i].lexeme;
      reading = false;
      types[typeName] = currentDefinitions;
      currentDefinitions = [];
    }
    if (reading && token.tokenClass == "IDENTIFIER") {
      let type = token.lexeme;
      let schemaType = type;
      let varName = eeprom_h[++i].lexeme;
      currentDefinitions.push({
        variable: varName,
        type: types[type] || type
      });
    }
  }
  //config_t is the root config
  return types.config_t;
}

//Generate a part of the config, by walking down the tree.
function generatePart(indent, root, partFunction, ...extra) {
  let indentStr = "\t".repeat(indent);
  if (Array.isArray(root)) {
    let genenerated = "{\n";
    for (let type of root) {
      genenerated += `${indentStr}${generatePart(indent + 1, type, partFunction, extra)}`;
    }
    return `${indentStr}${genenerated}};\n`;
  }
  if (Array.isArray(root.type)) {
    let genenerated = "{\n";
    for (let type of root.type) {
      genenerated += `${generatePart(indent + 1, type, partFunction, extra)}`;
    }
    return `${indentStr}${root.variable}: ${genenerated}${indentStr}},\n`;
  } else {
    return `${indentStr}${root.variable}: ${partFunction(root, extra)},\n`;
  }
}

function generateSchema(root) {
  return `_.type.${root.type}`;
}

function generateTypes(root) {
  let type = root.type;
  if (type.startsWith("uint") || type.startsWith("int")) {
    type = "number";
  } else if (type.startsWith("bool")) {
    type = "boolean";
  }
  return type;
}

function generateDefinitions(indent, root) {
  let schemaDefinitions = `export const GeneratedEEPROMConfig = ${generatePart(indent, root, generateSchema)}`;
  let typeDefinitions = `export type GeneratedEEPROMConfig = ${generatePart(indent, root, generateTypes)}`;
  return {schemaDefinitions, typeDefinitions};
}

async function generateBaseConfig(root) {
  let configDefinitions = [...root];
  let eeprom_c = parser.lexer.lexUnit.tokenize(await rp(`${config_url}/eeprom.c`));
  let reading = false;
  let current_config = [];
  for (let i = 0; i < eeprom_c.length; i++) {
    const token = eeprom_c[i];
    reading |= token.tokenClass == "{";
    if (token.tokenClass == "}") {
      break;
    }
    if (reading && token.tokenClass == "IDENTIFIER") {
      let config = configDefinitions.shift();
      current_config.push({type: config.type, variable: config.variable, value: token.lexeme});
    }
  }
  return current_config;
}

async function processDefaultsH() {
  let defaults_h = parser.lexer.lexUnit.tokenize(await rp(`${config_url}/defaults.h`));
  let defines = {};
  let data = [];
  let current;
  for (let i = 0; i < defaults_h.length; i++) {
    const token = defaults_h[i];
    //Skip preprocessor ifdefs
    if (token.lexeme.indexOf("if") != -1) {
      continue;
    }
    if (token.lexeme.indexOf("defined") != -1) {
      i += 3;
      continue;
    }
    if (token.lexeme == "define") {
      if (current) {
        defines[current] = data;
        data = [];
      }
      current = defaults_h[++i].lexeme;
    } else if (current) {
      data.push(token);
    }
  }
  defines[current] = data;
  //F_CPU is never actually defined, but we need to define it somewhere
  defines["F_CPU"] = [
    {
      lexeme: "-1",
      tokenClass: "CONSTANT"
    }
  ];
  return defines;
}

async function processEnumTypes() {
  const tsTypes = await new tsParser.TypescriptParser().parseFile("src/common/avr-types.ts", "workspace root");
  let enums = {};
  let enumMembers = {};
  for (let decl of tsTypes.declarations) {
    if (decl.members) {
      for (let member of decl.members) {
        const memberLC = member.toLowerCase();
        const declLC = decl.name.toLowerCase();
        enumMembers[memberLC] = `${decl.name}.${member}`;
        enumMembers[memberLC + declLC] = `${decl.name}.${member}`;
      }
    }
  }
  return enumMembers;
}
async function processDefaults(indent, type, tokens, enumMembers, defaults) {
  let indentStr = "\t".repeat(indent);
  if (type.type instanceof Array) {
    //We have a inner definition, so we should convert it
    let inner = "";
    for (let typeDef of type.type) {
      inner += await processDefaults(indent + 1, typeDef, tokens, enumMembers, defaults);
    }
    return `${indentStr}${type.variable}: {\n${inner}${indentStr}},\n`;
  } else {
    while (tokens[0].tokenClass != "CONSTANT" && tokens[0].tokenClass != "IDENTIFIER") {
      tokens.shift();
    }
    let value = tokens.shift().lexeme;
    if (defaults[value]) {
      value = defaults[value][0].lexeme;
    }
    const valueLC = value.toLowerCase().replace(/_/g, "");
    value = enumMembers[valueLC] || value;
    return `${indentStr}${type.variable}: ${value},\n`;
  }
}
async function generateDefaultConfig(root) {
  let enumMembers = await processEnumTypes();
  let defaults = await processDefaultsH();
  let currentConfig = await generateBaseConfig(root);
  let defaultConfig = "";
  for (let i = 0; i < currentConfig.length; i++) {
    let {value} = currentConfig[i];
    defaultConfig += await processDefaults(1, currentConfig[i], defaults[value], enumMembers, defaults);
  }
  return defaultConfig;
}
async function convert() {
  let eeprom_c = parser.lexer.lexUnit.tokenize(await rp(`${config_url}/eeprom.c`));
  let defines_h = parser.lexer.lexUnit.tokenize(await rp(`${config_url}/defines.h`));
  let root = await generateConfigTree();
  let {schemaDefinitions, typeDefinitions} = await generateDefinitions(0, root);
  let defaultConfig = await generateDefaultConfig(root);

  fs.writeFileSync("src/common/generated.ts", typeDefinitions);
  fs.writeFileSync("src/main/generated.ts", `import * as _ from 'c-struct';
import { DeviceType, EepromConfig, OutputType, InputType, TiltSensor, Subtype, GyroOrientation, PinConstants } from '../common/avr-types';
${schemaDefinitions}
export var defaultConfig: EepromConfig = {
${defaultConfig}};`);
}

convert();
