declare module "chip.avr.avr109" {
  import * as SerialPort from "serialport";
  export class Flasher {
    erase(fn : (err: Error) => void): void;
    program(data : string, fn : (err : Error) => void): void;
    verify(fn : (err : Error) => void): void;
    fuseCheck(fn : (err : Error) => void): void;
  }
  export function init(serialport : SerialPort, options : any, fn : (err : Error, flasher : Flasher) => void): void;
}
