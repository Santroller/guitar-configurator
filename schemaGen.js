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
  //config_t is the root config, and it contains the rest of the tree as a result.
  return types.config_t;
}

//Generate a part of the config, by walking down the tree.
function generatePart(indent, root, partFunction, extra) {
  let indentStr = "\t".repeat(indent);
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

//Pass this into generatePart to generate a schema
function generateSchema(root) {
  return `_.type.${root.type}`;
}

//Pass this into generatePart to generate a typescript definition
function generateTypes(root) {
  let type = root.type;
  if (type.startsWith("uint") || type.startsWith("int")) {
    type = "number";
  } else if (type.startsWith("bool")) {
    type = "boolean";
  }
  return type;
}

//Pass this into generatePart to generate a default eeprom config
function generateDefault(root, extra) {
  const {tokens, enumMembers, defaults} = extra;
  while (tokens[0].tokenClass != "CONSTANT" && tokens[0].tokenClass != "IDENTIFIER") {
    tokens.shift();
  }
  let value = tokens.shift().lexeme;
  if (defaults[value]) {
    value = defaults[value][0].lexeme;
  }
  const valueLC = value.toLowerCase().replace(/_/g, "");
  return (value = enumMembers[valueLC] || value);
}

//Generate both schema and typescript definitions
function generateDefinitions(indent, root) {
  let schemaDefinitions = "export const EepromSchema = {\n";
  let typeDefinitions = "export type config_t = {\n";
  for (let type of root) {
    schemaDefinitions += `${generatePart(1, type, generateSchema)}`;
    typeDefinitions += `${generatePart(1, type, generateTypes)}`;
  }
  schemaDefinitions += `};\n`;
  typeDefinitions += `};\n`;
  return {schemaDefinitions, typeDefinitions};
}

//merge together eeprom.c and the root tree, to get a version of eeprom.c with types
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

//Process defaults.h and build up a list of all defines
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

//Process avr-types and build a list of enum types that we can easily substitute in later
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

//Use everything we have build above to walk through most files and build a default configuration based on eeprom.c
async function generateDefaultConfig(root) {
  let enumMembers = await processEnumTypes();
  let defaults = await processDefaultsH();
  let currentConfig = await generateBaseConfig(root);
  let defaultConfig = "export const defaultConfig: EepromConfig = {\n";
  for (let i = 0; i < currentConfig.length; i++) {
    let {value} = currentConfig[i];
    defaultConfig += await generatePart(1, currentConfig[i], generateDefault, {
      tokens: defaults[value],
      enumMembers,
      defaults
    });
  }
  return defaultConfig +"};";
}

//Process the Ardwiino source + avr-types and spit out a few typescript files that we can use in the rest of the code base.
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
${defaultConfig}
`);
}

convert();
