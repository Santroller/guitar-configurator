import * as _ from "c-struct";
import {
  EepromConfig,
  OutputType,
  InputType,
  TiltSensor,
  Subtype,
  GyroOrientation,
  PinConstants
} from "../common/avr-types";
export const pins_t = {
  up: _.type.uint8,
  down: _.type.uint8,
  left: _.type.uint8,
  right: _.type.uint8,
  start: _.type.uint8,
  back: _.type.uint8,
  left_stick: _.type.uint8,
  right_stick: _.type.uint8,
  LB: _.type.uint8,
  RB: _.type.uint8,
  home: _.type.uint8,
  unused: _.type.uint8,
  a: _.type.uint8,
  b: _.type.uint8,
  x: _.type.uint8,
  y: _.type.uint8,
  lt: _.type.uint8,
  rt: _.type.uint8,
  l_x: _.type.uint8,
  l_y: _.type.uint8,
  r_x: _.type.uint8,
  r_y: _.type.uint8,
  mpu_6050_interrupt: _.type.uint8
};
export const axis_invert_t = {
  lt: _.type.bool,
  rt: _.type.bool,
  l_x: _.type.bool,
  l_y: _.type.bool,
  r_x: _.type.bool,
  r_y: _.type.bool
};
export const analogue_keys_t = {
  neg: _.type.uint8,
  pos: _.type.uint8
};
export const keys_t = {
  up: _.type.uint8,
  down: _.type.uint8,
  left: _.type.uint8,
  right: _.type.uint8,
  start: _.type.uint8,
  back: _.type.uint8,
  left_stick: _.type.uint8,
  right_stick: _.type.uint8,
  LB: _.type.uint8,
  RB: _.type.uint8,
  home: _.type.uint8,
  a: _.type.uint8,
  b: _.type.uint8,
  x: _.type.uint8,
  y: _.type.uint8,
  lt: _.type.uint8,
  rt: _.type.uint8,
  l_x: analogue_keys_t,
  l_y: analogue_keys_t,
  r_x: analogue_keys_t,
  r_y: analogue_keys_t
};
export const config_t = {
  version: _.type.uint8,
  output_type: _.type.uint8,
  input_type: _.type.uint8,
  tilt_type: _.type.uint8,
  subtype: _.type.uint8,
  pollrate: _.type.uint8,
  pins: pins_t,
  mpu_6050_orientation: _.type.uint16,
  frets_led_mode: _.type.bool,
  map_joy_to_dpad: _.type.bool,
  inversions: axis_invert_t,
  threshold_trigger: _.type.uint16,
  threshold_joy: _.type.uint16,
  keys: keys_t,
  cpu_freq: _.type.uint32,
  signature: _.type.uint32
};

export var defaultConfig: EepromConfig = {
  version: 0,
  output_type: OutputType.XInput,
  input_type: InputType.Direct,
  tilt_type: TiltSensor.MPU6050,
  subtype: Subtype.Guitar,
  pollrate: 1,
  pins: {
    up: 14,
    down: 15,
    left: PinConstants.InvalidPin,
    right: PinConstants.InvalidPin,
    start: 16,
    back: 9,
    left_stick: PinConstants.InvalidPin,
    right_stick: PinConstants.InvalidPin,
    LB: 8,
    RB: PinConstants.InvalidPin,
    home: PinConstants.InvalidPin,
    unused: PinConstants.InvalidPin,
    a: 4,
    b: 5,
    x: 6,
    y: 7,
    lt: PinConstants.InvalidPin,
    rt: PinConstants.InvalidPin,
    l_x: 19,
    l_y: 20,
    r_x: 18,
    r_y: PinConstants.InvalidPin,
    mpu_6050_interrupt: 2
  },
  mpu_6050_orientation: GyroOrientation.NegativeX,
  frets_led_mode: true,
  map_joy_to_dpad: true,
  inversions: {
    lt: false,
    rt: false,
    l_x: false,
    l_y: false,
    r_x: false,
    r_y: false
  },
  threshold_trigger: 12767,
  threshold_joy: 12767,
  keys: {
    up: 0x04,
    down: 0x16,
    left: 0x0d,
    right: 0x0e,
    start: 0x0f,
    back: 0x33,
    left_stick: 0x28,
    right_stick: 0x0b,
    LB: 0x50,
    RB: 0x4f,
    home: 0x52,
    a: 0x51,
    b: 0x51,
    x: PinConstants.InvalidPin,
    y: PinConstants.InvalidPin,
    lt: PinConstants.InvalidPin,
    rt: PinConstants.InvalidPin,
    l_x: {
      neg: PinConstants.InvalidPin,
      pos: PinConstants.InvalidPin
    },
    l_y: {
      neg: PinConstants.InvalidPin,
      pos: PinConstants.InvalidPin
    },
    r_x: {
      neg: PinConstants.InvalidPin,
      pos: PinConstants.InvalidPin
    },
    r_y: {
      neg: PinConstants.InvalidPin,
      pos: PinConstants.InvalidPin
    }
  },
  cpu_freq: -1,
  signature: Subtype.Guitar
};
