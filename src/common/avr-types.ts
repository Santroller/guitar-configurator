import {config_t} from "./generated";
export interface Board {
  name: string;
  baud: number;
  productId: string[];
  protocol: string;
  processor: string;
  com?: string;
  manufacturer?: string;
  cleanName: string;
}
export type ProgressCallback = (percentage : number, state : string) => void;
export type Guitar = {
  type: DeviceType;
  config: EepromConfig;
  board: Board;
  updating: boolean;
};
export enum Subtype {
  Gamepad = 1,
  Wheel,
  ArcadeStick,
  FlightStick,
  DancePad,
  Guitar,
  GuitarAlternate,
  Drum,
  GuitarBass = 11,
  ArdcadePad = 19
}
export enum TiltSensor {
  None,
  MPU6050,
  Gravity
}
export enum InputType {
  None,
  Wii,
  Direct = 2
}
export enum OutputType {
  XInput = 1,
  Keyboard,
  Gamepad
}
export enum DeviceType {
  Guitar = 0xfea123,
  FlameWake = 0xf1a3e48,
  Unprogrammed = 0x00
}
export enum GyroOrientation {
  PositiveZ = 0,
  NegativeZ = 1,
  PositiveY = 2,
  NegativeY = 3,
  PositiveX = 4,
  NegativeX = 5
}

export enum PinConstants {
  InvalidPin = 0xff
}

//Swap any non-number types for their enum variants.
export type EepromConfig = config_t & {
  output_type: OutputType;
  input_type: InputType;
  tilt_type: TiltSensor;
  subtype: Subtype;
  mpu_6050_orientation: GyroOrientation;
  [key: string]: any;
};
