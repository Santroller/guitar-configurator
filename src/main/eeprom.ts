import * as _ from "c-struct";
import * as hex from "node-intelhex";

import { EepromConfig } from "../common/avr-types";

import { EepromSchema, defaultConfig } from "./generated";
export {
  defaultConfig
};
_.register("EepromConfig", new _.Schema(EepromSchema));
export const EepromConfigLength = generateEEP(defaultConfig).length;
export function readData(data: Buffer): EepromConfig {
  return _.unpackSync("EepromConfig", data);
}

export function writeData(data: EepromConfig): Buffer {
  return _.packSync("EepromConfig", data);
}

export function generateEEP(data: EepromConfig) {
  hex.setLineBytes(16);
  let reader = hex.bufferReader(0x00, writeData(data));
  let ret = "";
  while (!reader.eof()) {
    ret += reader.getNextRecord() + "\n";
  }
  return ret.toUpperCase();
}
