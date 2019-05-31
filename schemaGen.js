// Generate TS classes based upon the C code from the Ardwiino lib
const rp = require("request-promise");
const fs = require("fs");
const parser = require("node-c-parser");
const tsParser = require("typescript-parser");
const config_url = "https://raw.githubusercontent.com/sanjay900/Ardwiino/master/src/shared/config/";
async function generateTypes() {
  let eeprom_h = parser.lexer.lexUnit.tokenize(await rp(`${config_url}/eeprom.h`));
  let schemas = "";
  let types = "";
  let currentSchema = "";
  let currentType = "";
  let reading = false;
  let foundTypes = [];
  let typeDefinitions = {};
  let currentDefinitions = [];
  for (let i = 0; i < eeprom_h.length; i++) {
    const token = eeprom_h[i];
    if (token.lexeme == "struct") {
      reading = true;
    }
    if (reading && token.lexeme == "ATTR_PACKED") {
      const typeName = eeprom_h[++i].lexeme;
      reading = false;
      foundTypes.push(typeName);
      schemas += `export const ${typeName} = {\n${currentSchema}}\n`;
      types += `export type ${typeName} = {\n${currentType}}\n`;
      typeDefinitions[typeName] = currentDefinitions;
      currentSchema = "";
      currentType = "";
      currentDefinitions = [];
    }
    if (reading && token.tokenClass == "IDENTIFIER") {
      let type = token.lexeme;
      let schemaType = type;
      let varName = eeprom_h[++i].lexeme;
      if (foundTypes.indexOf(type) == -1) {
        schemaType = `_.type.${type}`;
      }
      currentSchema += `  ${varName}: ${schemaType},\n`;
      if (type.startsWith("uint")) {
        type = "number";
      } else if (type.startsWith("bool")) {
        type = "boolean";
      }
      currentDefinitions.push({varName, type});
      currentType += `\t${varName}: ${type},\n`;
    }
  }
  return {schemas, types, typeDefinitions};
}

async function generateBaseConfig(typeDefinitions) {
  let configDefintions = [...typeDefinitions.config_t];
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
      let config = configDefintions.shift();
      current_config.push({type: config.type, variable: config.varName, value: token.lexeme});
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
async function processDefaults(indent, type, tree, enumMembers, typeDefinitions, defaults) {
  let indentStr = "\t".repeat(indent);
  while (tree[0].tokenClass != "CONSTANT" && tree[0].tokenClass != "IDENTIFIER") {
    tree.shift();
  }
  if (typeDefinitions[type.type]) {
    //We have a inner definition, so we should convert it
    let inner = "";
    for (let typeDef of typeDefinitions[type.type]) {
      inner += await processDefaults(indent + 1, typeDef, tree, enumMembers, typeDefinitions, defaults);
    }
    return `${indentStr}${type.varName}: {\n${inner}${indentStr}},\n`;
  } else {
    let value = tree.shift().lexeme;
    if(defaults[value]) {
      value = defaults[value][0].lexeme;
    }
    const valueLC = value.toLowerCase().replace(/_/g, "");
    value = enumMembers[valueLC] || value;
    return `${indentStr}${type.varName}: ${value},\n`;
  }
}
async function generateDefaults(typeDefinitions) {
  let enumMembers = await processEnumTypes();
  let defaults = await processDefaultsH();
  let current_config = await generateBaseConfig(typeDefinitions);
  let defaultTS = "";
  for (let i = 0; i < current_config.length; i++) {
    let {variable, value} = current_config[i];
    if (value == "F_CPU") {
      defaultTS += `${variable}: -1,\n`;
      continue;
    }
    defaultTS += await processDefaults(0, typeDefinitions.config_t[i], defaults[value], enumMembers, typeDefinitions, defaults);
  }
  return defaultTS;
}
async function convert() {
  let eeprom_c = parser.lexer.lexUnit.tokenize(await rp(`${config_url}/eeprom.c`));
  let defines_h = parser.lexer.lexUnit.tokenize(await rp(`${config_url}/defines.h`));
  let {schemas, types, typeDefinitions} = await generateTypes();

  let defaultTS = await generateDefaults(typeDefinitions);
  fs.writeFileSync("src/common/generated.ts", types);
  fs.writeFileSync("src/main/generated.ts", `import * as _ from 'c-struct';
import { DeviceType, EepromConfig, OutputType, InputType, TiltSensor, Subtype, GyroOrientation, PinConstants } from '../common/avr-types';
${schemas}
export var defaultConfig: EepromConfig = {
${defaultTS}
};`);
}

convert();
