
import * as SerialPort from 'serialport';
import {Board} from '../common/avr-types';
var uno: Board = {
  name: 'uno-usb',
  hex: 'uno-usb',
  baud: 57600,
  productId: [],
  protocol: 'avr109',
  processor: 'atmega16u2',
  cleanName: 'Arduino Uno',
  cpuFrequency: 16000000,
  hasBootloader: true,
  signature: [0x1e, 0x94, 0x89]
};
export var boards: {[name: string]: Board} = {
  'uno-usb-16u2': {
    ...uno,
    name: 'uno-usb-16u2',
    processor: 'atmega16u2',
    signature: [0x1e, 0x94, 0x89]
  },
  'uno-usb-8u2': {
    ...uno,
    name: 'uno-usb-8u2',
    processor: 'atmega8u2',
    signature: [0x1e, 0x93, 0x89]
  },
  'uno-main': {
    hex: 'uno-main',
    name: 'uno-main',
    baud: 115200,
    productId: ['0043', '7523', '0001', 'ea60'],
    protocol: 'arduino',
    processor: 'atmega328p',
    cleanName: 'Arduino Uno',
    cpuFrequency: 16000000,
    hasBootloader: false,
    signature: [0x1e, 0x95, 0x0F]
  },
  'micro': {
    hex: 'micro',
    name: 'micro',
    baud: 57600,
    productId: [
      '0037', '8037', '0036', '0237', '9206', '9207', '9205', '8036', '800c'
    ],
    protocol: 'avr109',
    processor: 'atmega32u4',
    cleanName: 'Arduino Micro',
    hasBootloader: true,
    signature: [0x1e, 0x95, 0x87]
  }
};

export function getAvrdudeArgs(board: Board): string[] {
  return [
    `-p${board.processor}`, `-c${board.protocol}`, `-b${board.baud}`,
    `-P${board.com}`
  ];
}

export async function findConnectedDevice(): Promise<Board|undefined> {
  let ports = await SerialPort.list();
  for (let port of ports) {
    for (let board of Object.values(boards)) {
      if (board.productId.indexOf(port.productId!) != -1) {
        board.com = port.comName;
        // At this point in time, we do not know if the bootloader is valid on
        // boards with multiple processors.
        board.hasBootloader = !board.name.includes('-');
        return board;
      }
    }
  }
  return undefined;
}

export function hasMultipleChips(board: Board) {
  return board.name.includes('-');
}
