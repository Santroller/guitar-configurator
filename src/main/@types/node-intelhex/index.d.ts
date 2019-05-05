declare module "node-intelhex" {
  export type Options = {
    progress?: (progress : number) => {};
  };

  export type InfoOptions = {
    progress?: (progress : number) => {};
    info?: (info : string) => {};
  };

  export interface BufferReader {
      getNextRecord(): string;
      eof(): boolean;
      length(): number;
      bytesRead(): number;
  }

  export function bufferReader(address : number, data : Buffer): BufferReader;

  export function readFile(filename : string, options : InfoOptions, cb : () => {}): any;

  export function setLineBytes(mlb : number): void;

  export function writeFile(filename : string, options : Options, cb : () => {}): any;
}
