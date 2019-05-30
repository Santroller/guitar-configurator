// Generate TS classes based upon the C code from the Ardwiino lib
let rp = require("request-promise");
let fs = require("fs");

// Convert a single line from a c struct decleration to a c-struct decleration
function schemafy(c) {
  return c.replace(/uint(\S+)_t ([^;]+);/g, "$2: _.type.uint$1,").replace(/bool ([^;]+);/g, "$1: _.type.bool,").replace(/(\S+)_t ([^;]+);/g, "$2: $1_t,");
}

// Convert a single line from a c struct decleration to a typescript
function typeify(c) {
  return c.replace(/uint(\S+)_t ([^;]+);/g, "$2: number,").replace(/bool ([^;]+);/g, "$1: boolean,").replace(/(\S+)_t ([^;]+);/g, "$2: $1_t,");
}

// Convert a variable definition to its typescript variant
function processDef(varAssignment, varType, ts) {
  if (varAssignment.indexOf("{") != -1) {
    let type = new RegExp(`export type ${varType} = ([^;]+);`).exec(ts)[1];
    let data = varAssignment.slice(1);
    while (type.indexOf("number") != -1 || type.indexOf("boolean") != -1 || type.indexOf("_t") != -1) {
      let next = /(number|boolean|(\S+_t))/.exec(type)[1];
      //We have found an embedded definition, so we call this function again to process it.
      if (next.endsWith("_t")) {
        type = type.replace(next, processDef(data.slice(0, data.indexOf("}") + 1), next, ts));
        data = data.slice(data.indexOf("}") + 2);
      } else {
        type = type.replace(next, data.slice(1, data.indexOf(",")));
        data = data.slice(data.indexOf(",") + 1);
      }
    }
    return type;
  } else {
    return varAssignment;
  }
}

async function convert() {
  let data = await rp("https://raw.githubusercontent.com/sanjay900/Ardwiino/master/src/shared/config/eeprom.h");
  let cEEPROMData;
  let re = /typedef struct ({[^}\r]+}) ATTR_PACKED ([^;]+);/g;
  let c_struct_ts = "";
  let ts_types = "";
  while ((cEEPROMData = re.exec(data))) {
    c_struct_ts += `export const ${cEEPROMData[2]} = ${schemafy(cEEPROMData[1])};\n`;
    ts_types += `export type ${cEEPROMData[2]} = ${typeify(cEEPROMData[1])};\n`;
  }
  fs.writeFileSync("src/common/generated.ts", ts_types);
  let defines = await rp("https://raw.githubusercontent.com/sanjay900/Ardwiino/master/src/shared/config/defines.h");
  let defaults = await rp("https://raw.githubusercontent.com/sanjay900/Ardwiino/master/src/shared/config/defaults.h");
  data = await rp("https://raw.githubusercontent.com/sanjay900/Ardwiino/master/src/shared/config/eeprom.c");
  let conf = /export type config_t = ([^;]+);/.exec(ts_types);
  //Look for variable definitions in both the typescript types, and the c eeprom
  re = /([^ {},]+)[,}]/g;
  re2 = /([^ {},]+): ([^{},]+),/g;
  let tsTypeData;
  let output = "";
  //Loop through both the typescript and c eeprom at the same time, as we need info from both files
  while ((cEEPROMData = re.exec(data)) && (tsTypeData = re2.exec(conf))) {
    let definition = new RegExp(`#\\s*define ${cEEPROMData[1]} ([^#]+)`).exec(defaults);
    if (definition) {
      // Convert from prettyprinted newline to a single line
      definition = definition[1].replace(/\s*\\\s*\n\s*/g, " ").trim();
      // Drop any other data (aka comments) on other lines
      if (definition.indexOf("\n") > 0) {
        definition = definition.slice(0, definition.indexOf("\n"));
      }
      // The c structs don't store field names, so we have to bring that back
      // again. We do this by pulling in field names from the previous converted typescript file.
      output += `${tsTypeData[1]}: ${processDef(definition, tsTypeData[2], ts_types)},\n`;
    } else {
      // F_CPU is never defined, set a nonsensical default so we can check it
      // is defined later.
      output += `${tsTypeData[1]}: -1,\n`;
    }
  }
  //Look for a basic variable decleration that will probably be a ifdef definition
  re = /([A-Z][A-Z_0-9]+)(?:[, ]|$)/gm;
  let types = fs.readFileSync("src/common/avr-types.ts") + "";
  let ifdef;
  tsTypeData = [];
  // Read every enum type from avr-types
  while ((ifdef = re.exec(output))) {
    tsTypeData.push(ifdef);
  }
  // Loop through and replace ifdef defines with their enum counterparts.
  for (ifdef of tsTypeData) {
    //Try to find a matching definition for a variable
    let definition = new RegExp(`#\\s*define ${ifdef[1]} ([^#]+)`).exec(defaults);
    if (definition) {
      definition = definition[1];
      if (definition.indexOf("\n") > 0) {
        definition = definition.slice(0, definition.indexOf("\n"));
      }
      output = output.replace(ifdef[1], definition);
    } else {
      //No ifdef was found in the defaults file. This means we are probably dealing with something out of the defines.h file.
      //Instead of hardcoding constants from that file, look for the varible in one of the manually defined enums in avr-types
      let definition = new RegExp(`export enum ([^ ]+) {[^}]*(${ifdef[1].replace("_", "")})[^}]*`, "im").exec(types);
      //Some things (such as subtypes) end with an _subtype, that doesn't exist in the enum, so we erase these if we don't find a match above.
      definition = definition || new RegExp(`export enum ([^ ]+) {[^}]*(${ifdef[1].slice(0, ifdef[1].lastIndexOf("_")).replace("_", "")})[^}]*`, "im").exec(types);
      if (definition) {
        //Swap the ifdef for a enum definition
        output = output.replace(ifdef[1], `${definition[1]}.${definition[2]}`);
      }
    }
  }
  //Generate the file. Add some required imports and then dump in the generated output.
  fs.writeFileSync("src/main/generated.ts", `import * as _ from 'c-struct';
import { EepromConfig, OutputType, InputType, TiltSensor, Subtype, GyroOrientation, PinConstants } from '../common/avr-types';
${c_struct_ts}
export var defaultConfig: EepromConfig = {\n${output}\n};`);
}

convert();
