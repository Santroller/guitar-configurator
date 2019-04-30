import * as _ from 'c-struct';
export type EepromConfig = {
  output_type: number; input_type: number; tilt_type: number; subtype: number;
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
  direction_mode: number;
  whammy_calibration: number;
  mpu_6050_calibration: number;
  frets_led_mode: number;
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
};
var eepromSchema = new _.Schema({
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
  }
});
_.register('EepromConfig', eepromSchema);

export function readData(data: Buffer): EepromConfig {
  return _.unpackSync('EepromConfig', data);
}

export function writeData(data: EepromConfig): Buffer {
  return _.packSync('EepromConfig', data);
}
