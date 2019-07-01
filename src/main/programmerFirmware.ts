
import * as delay from 'delay';
import * as fs from 'fs';
import {IPty} from 'node-pty';
import * as path from 'path';
import * as SerialPort from 'serialport';
import * as tmp from 'tmp';
import * as usb from 'usb';

import {Board, DeviceType, Guitar, ProgressCallback} from '../common/avr-types';

import {boards, findConnectedDevice, getAvrdudeArgs} from './boards';
import {defaultConfig, generateEEP} from './eeprom';
import {controlTransfer, executeBinary, findBinary} from './programmerUtils';
import {restoreController} from './programmerWindows';

declare const __static: string;

export enum AvrFileFormat {
  IntelHex = 'i',
  MotorolaSRecord = 's',
  RawBinary = 'r',
  Elf = 'e',
  Immediate = 'm',
  AutoDetect = 'a',
  Decimal = 'd',
  Hex = 'h',
  Octal = 'o',
  Binary = 'b'
}
export enum MemoryLocation {
  FLASH = 'flash',
  EEPROM = 'eeprom'
}
export enum MemoryAction {
  READ = 'r',
  WRITE = 'w'
}
function avrdudeArgs(
    location: MemoryLocation, action: MemoryAction, file: string) {
  if (action == MemoryAction.READ) {
    return ['-U', `${location}:${action}:${file}:${AvrFileFormat.RawBinary}`];
  } else {
    return ['-U', `${location}:${action}:${file}:${AvrFileFormat.AutoDetect}`];
  }
}
function getProgress(progress: ProgressCallback) {
  let loc = 0;
  return (chunk: string) => {
    if (chunk.includes('writing') && loc != 0) {
      loc += 50;
    }
    if (chunk.includes('reading on-chip')) {
      loc += 50;
    }
    let data = /(Writing|Reading) \| #+\s+\| (\d+)% (\d.\d+s)/g.exec(chunk);
    if (data) {
      let p = parseInt(data[2]);
      p /= 2;
      progress(p + loc, 'avrdude');
    }
  }
}
async function spawnAvrDude(
    args: string[], board: Board, onData: (chunk: string, proc: IPty) => void) {
      console.log(args);
  return executeBinary(
      'avrdude',
      ['-C', findBinary('avrdude.conf'), ...getAvrdudeArgs(board), ...args, "reset"],
      onData);
}

/**
 * Work out what kind of board is connected. Only used for detecting hoodloader
 * boards.
 */
export async function getConnectedBoard(board: Board): Promise<Board> {
  return await new Promise(resolve => {
    let port = new SerialPort(board.com!, {baudRate: 57600});
    port.on('data', data => {
      port.close();
      let signature = [...data].reverse();
      // Find a board with a matching signature
      let found = Object.values(boards).find(
          s => s.signature.every((v, i) => v == signature[i]));
      if (found) {
        found.com = board.com;
        resolve(found);
      } else {
        resolve(board);
      }
    });
    // Request for the device signature
    port.write('s');
    // If we don't have a hoodloader device connected, this will timeout.
    setTimeout(() => {
      if (port.isOpen) {
        port.close();
        resolve(board)
      }
    }, 500);
  });
}
export function findFirmware(type: string, board: Board) {
  return path.join(
      __static, 'firmwares', type,
      `${type}-${board.hex}-${board.processor}-${board.cpuFrequency}.hex`);
}
// If we pass in a frequency of zero, we are ignoring the freq parameter.
export async function program(
    device: string, guitar: Guitar, progress: ProgressCallback) {
  if (!guitar.board) return;
  let board = boards[device];
  board.com = guitar.board.com;
  let args: string[] = [];
  //On first write, we need to write a default configuration. We only need to write a config to the first controller however.
  if (guitar.updating && !device.endsWith('-main')) {
    if (!device.endsWith('-main')) {
      const file_eep = tmp.fileSync();
      const stream_eep = fs.createWriteStream(null!, {fd: file_eep.fd});
      stream_eep.write(generateEEP(guitar.config));
      args =
          avrdudeArgs(MemoryLocation.EEPROM, MemoryAction.WRITE, file_eep.name);
    }
  }
  args = args.concat(avrdudeArgs(
      MemoryLocation.FLASH, MemoryAction.WRITE,
      findFirmware('ardwiino', board)));
  await spawnAvrDude(args, board, getProgress((p, s) => {
                       if (guitar.updating && !device.endsWith('-main')) {
                         progress(p / 2, s);
                       } else {
                         progress(p, s);
                       }
                     }));

  await restoreController(guitar);
}

export async function programHoodloader(
    guitar: Guitar, progress: ProgressCallback) {
  if (!guitar.board) return;
  guitar.board.cpuFrequency =
      guitar.board.cpuFrequency || guitar.config.cpu_freq;
  await spawnAvrDude(
      avrdudeArgs(
          MemoryLocation.FLASH, MemoryAction.WRITE,
          findFirmware('hoodloader', guitar.board)),
      guitar.board, getProgress((p, s) => progress(p, s)));
}

export async function searchForProgrammer(): Promise<Guitar|undefined> {
  let board = await findConnectedDevice();
  if (!board) return;
  // Detect unos using hoodloader
  board = await getConnectedBoard(board);
  return {
    type: DeviceType.Unprogrammed,
    config: defaultConfig,
    updating: true,
    board
  };
}
async function jumpToBootloader() {
  await controlTransfer(0x30, usb.LIBUSB_ENDPOINT_IN, 0);
  await delay(500);
}
export async function findAndJumpBootloader(): Promise<Board> {
  await jumpToBootloader();
  let board = await findConnectedDevice();
  if (board) {
    return board;
  }
  await delay(500);
  return findAndJumpBootloader();
}