export enum MemoryLocation {
  FLASH = "flash",
  EEPROM = "eeprom"
}
export interface Board {
  name: string;
  baud: number;
  productId: string[];
  protocol: string;
  processor: string;
  com?: string;
}
export type ProgressCallback = (percentage : number, state: string) => void;
export type Guitar = {
  type: DeviceType;
  config: EepromConfig;
  board: Board;
};
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
export enum DeviceType {
  Guitar=0xFEA123,
  FlameWake=0xF1A3E48,
  Unprogrammed=0x00
}
export type EepromConfig = {
  protocol_version: number;
  output_type: OutputType;
  input_type: InputType;
  tilt_type: TiltSensor;
  subtype: Subtype;
  pollrate: number;
  pins: {
    green: number;
    red: number;
    yellow: number;
    blue: number;
    orange: number;
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
    green: number;
    red: number;
    yellow: number;
    blue: number;
    orange: number;
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
