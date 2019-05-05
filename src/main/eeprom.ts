import * as _ from 'c-struct';
import * as hex from 'node-intelhex';

export enum Subtype {
  Gamepad = 1,
  Wheel,
  ArcadeStick,
  ArcadePad,
  FlightStick,
  DancePad,
  Guitar,
  GuitarAlternate,
  Drum,
  GuitarBass = 11,
  ArdcadePad = 10
}
export enum TiltSensor {
  None,
  MPU6050,
  Switch
}
export enum InputType {
  None,
  Wii,
  Direct = 2
}
export enum OutputType {
  Serial,
  XInput,
  Keyboard,
  Gamepad
}
export enum JoyType {
  None,
  Dpad,
  Joy
}
export type EepromConfig = {
  protocol_version: number; output_type: OutputType; input_type: InputType;
  tilt_type: TiltSensor;
  subtype: Subtype;
  pollrate: number;
  pins: {
    green: number; red: number; yellow: number; blue: number; orange: number;
    start: number;
    select: number;
    whammy: number;
    strum_up: number;
    strum_down: number;
    dpad_left: number;
    dpad_right: number;
    joy_x: number;
    joy_y: number;
    gravity: number;
  };
  direction_mode: JoyType;
  whammy_calibration: number;
  mpu_6050_calibration: number;
  frets_led_mode: boolean;
  keys: {
    green: number; red: number; yellow: number; blue: number; orange: number;
    up: number;
    down: number;
    left: number;
    right: number;
    start: number;
    select: number;
    whammy: number;
  };
  cpu_freq: number;
  signature: number;
};
var eepromSchema = new _.Schema({
  protocol_version: _.type.uint8,
  output_type: _.type.uint8,
  input_type: _.type.uint8,
  tilt_type: _.type.uint8,
  subtype: _.type.uint8,
  pollrate: _.type.uint8,
  pins: {
    green: _.type.uint8,
    red: _.type.uint8,
    yellow: _.type.uint8,
    blue: _.type.uint8,
    orange: _.type.uint8,
    start: _.type.uint8,
    select: _.type.uint8,
    whammy: _.type.uint8,
    strum_up: _.type.uint8,
    strum_down: _.type.uint8,
    dpad_left: _.type.uint8,
    dpad_right: _.type.uint8,
    joy_x: _.type.uint8,
    joy_y: _.type.uint8,
    gravity: _.type.uint8
  },
  direction_mode: _.type.uint8,
  whammy_calibration: _.type.uint16,
  mpu_6050_calibration: _.type.uint16,
  frets_led_mode: _.type.uint8,
  keys: {
    green: _.type.uint8,
    red: _.type.uint8,
    yellow: _.type.uint8,
    blue: _.type.uint8,
    orange: _.type.uint8,
    up: _.type.uint8,
    down: _.type.uint8,
    left: _.type.uint8,
    right: _.type.uint8,
    start: _.type.uint8,
    select: _.type.uint8,
    whammy: _.type.uint8
  },
  cpu_freq: _.type.uint32,
  signature: _.type.uint32
});
_.register('EepromConfig', eepromSchema);

export function readData(data: Buffer): EepromConfig {
  return _.unpackSync('EepromConfig', data);
}

export function writeData(data: EepromConfig): Buffer {
  return _.packSync('EepromConfig', data);
}

export function generateEEP(data: EepromConfig) {
  hex.setLineBytes(16);
  let reader = hex.bufferReader(0x00, writeData(data));
  let ret = '';
  while (!reader.eof()) {
    ret += reader.getNextRecord() + '\n';
  }
  return ret.toUpperCase();
}

export var defaultConfig: EepromConfig = {
  protocol_version: 0,
  output_type: OutputType.XInput,
  input_type: InputType.None,
  tilt_type: TiltSensor.None,
  subtype: Subtype.Guitar,
  pollrate: 1,
  pins: {
    green: 4,
    red: 5,
    yellow: 7,
    blue: 6,
    orange: 8,
    start: 16,
    select: 9,
    whammy: 18,
    strum_up: 14,
    strum_down: 15,
    dpad_left: 10,
    dpad_right: 21,
    joy_x: 19,
    joy_y: 20,
    gravity: 11,
  },
  direction_mode: JoyType.None,
  whammy_calibration: 0,
  mpu_6050_calibration: 0,
  frets_led_mode: false,
  keys: {
    green: 0x04,
    red: 0x16,
    yellow: 0x0d,
    blue: 0x0e,
    orange: 0x0f,
    up: 0x33,
    down: 0x28,
    left: 0x0b,
    right: 0x50,
    start: 0x4f,
    select: 0x52,
    whammy: 0x51,
  },
  cpu_freq: 1600000,
  signature: 0xFEA123
}