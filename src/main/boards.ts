
import * as SerialPort from 'serialport';
import {jumpToBootloader} from './programmer';
import { Board } from '../common/avr-types';
//A list of board definitions. note that we don't actually ever want to autodetect the main uno processor, so it has no ids.
export var boards: {[name: string]: Board} = {
  'uno-usb': {
    name: 'uno-usb',
    baud: 57600,
    productId: ['0043', '7523', '0001', 'ea60'],
    protocol: 'avr109',
    processor: 'atmega16u2'
  },
  'uno-main': {
    name: 'uno-main',
    baud: 115200,
    productId: [],
    protocol: 'arduino',
    processor: 'atmega328p'
  },
  'micro': {
    name: 'micro',
    baud: 57600,
    productId: [
      '0037', '8037', '0036', '0237', '9206', '9207', '9205', '8036', '800c'
    ],
    protocol: 'avr109',
    processor: 'atmega32u4'
  }
};

export function getAvrdudeArgs(board: Board): string[] {
  return [
    `-p${board.processor}`, '-P', 'usb', `-c${board.protocol}`,
    `-b${board.baud}`, `-P${board.com}`
  ];
}

export async function findConnectedDevice(): Promise<Board|undefined> {
  await jumpToBootloader();
  await new Promise(resolve => setTimeout(resolve, 500));
  let ports = await SerialPort.list();
  for (let port of ports) {
    for (let board of Object.values(boards)) {
      if (board.productId.indexOf(port.productId!) != -1) {
        board.com = port.comName;
        return board;
      }
    }
  }
  return undefined;
}
