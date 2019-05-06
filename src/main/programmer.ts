import * as delay from 'delay';
import * as fs from 'fs';
import * as path from 'path';
import {spawn} from 'pty.js';
import * as rp from 'request-promise-native';
import * as tmp from 'tmp';
import * as usb from 'usb';

import {Board, EepromConfig, Guitar, MemoryLocation, ProgressCallback} from '../common/avr-types';

import {boards, findConnectedDevice, getAvrdudeArgs} from './boards';
import {defaultConfig, generateEEP, readData} from './eeprom';

export enum MemoryAction {
  READ = 'r',
  WRITE = 'w'
}

function findBinary(...args: string[]) {
  // When running from an asar, the files get extracted to one place, otherwise,
  // they are run directly
  if (process.mainModule!.filename.indexOf('app.asar') === -1) {
    return path.join(__dirname, '..', '..', 'binaries', ...args);
  }
  return path.join(__dirname, '..', 'binaries', ...args);
}
export function spawnAvrDude(
    args: string[], board: Board|null,
    progress: ProgressCallback): Promise<void> {
  return new Promise(async (resolve, reject) => {
    let connected = await findConnectedDevice();
    if (!connected) {
      reject('no device found');
      return;
    }
    // If no board is passed in, then we generate one based on what is plugged
    // in.
    board = board || connected;
    board.com = connected.com;
    const avrdudePath =
        findBinary('avrdude', process.platform + '-' + process.arch, 'avrdude');
    let proc = spawn(
        avrdudePath,
        ['-C', `${avrdudePath}.conf`, ...getAvrdudeArgs(board), ...args]);
    proc.on('exit', function(exitCode: number) {
      if (exitCode == 0) {
        resolve();
      } else {
        reject(`non-zero exit code: ${exitCode}`);
      }
    });

    proc.stdout.on('data', function(chunk: string) {
      let loc = MemoryLocation.EEPROM;
      if (chunk.indexOf('eeprom') != -1) {
        loc = MemoryLocation.EEPROM;
      }
      if (chunk.indexOf('flash') != -1) {
        loc = MemoryLocation.FLASH;
      }
      let data = /(Writing|Reading) \| #+\s+\| (\d+)% (\d.\d+s)/g.exec(chunk);
      if (data) {
        progress(loc, parseInt(data[2]), data[3]);
      }
    });
  });
}
function avrdudeMemoryArgs(
    location: MemoryLocation, action: MemoryAction, file: string) {
  return ['-U', `${location}:${action}:${file}`];
}
export async function readEeprom(progress: ProgressCallback):
    Promise<EepromConfig> {
  const file = tmp.fileSync();
  await spawnAvrDude(
      avrdudeMemoryArgs(MemoryLocation.EEPROM, MemoryAction.READ, file.name),
      null, progress);
  return readData(fs.readFileSync(file.name));
}
async function retrieveHex(file: string) {
  let release = await rp({
    url: 'https://api.github.com/repos/sanjay900/Ardwiino/releases/latest',
    json: true,
    headers: {'User-Agent': 'Node'}
  });
  const hex_url =
      release.assets.filter((s: any) => s.name == file)[0].browser_download_url;
  return await rp({uri: hex_url, encoding: null});
}
// If we pass in a frequency of zero, we are ignoring the freq parameter.
export async function program(
    device: string, freq: number, config: EepromConfig|null,
    progress: ProgressCallback) {
  const flash_data = await retrieveHex(`ardwiino-${device}${- freq || ''}.hex`);
  const file_flash = tmp.fileSync();
  const stream_flash = fs.createWriteStream(null!, {fd: file_flash.fd});
  stream_flash.write(flash_data);
  let args = avrdudeMemoryArgs(
      MemoryLocation.FLASH, MemoryAction.WRITE, file_flash.name);
  if (config != null) {
    const file_eep = tmp.fileSync();
    const stream_eep = fs.createWriteStream(null!, {fd: file_eep.fd});
    stream_eep.write(generateEEP(config));
    args = avrdudeMemoryArgs(
               MemoryLocation.EEPROM, MemoryAction.WRITE, file_eep.name)
               .concat(args);
  }
  await spawnAvrDude(args, boards[device], progress);
}

export function jumpToBootloader() {
  // win32 doesnt support control transfers.
  if (process.platform === 'win32') return;
  return new Promise((resolve, reject) => {
    try {
      let dev = usb.findByIds(0x1209, 0x2882);
      dev.open();
      // The ardwiino firmware responds to a control transfer of 0x30 by jumping
      // to the bootloader.
      dev.controlTransfer(usb.LIBUSB_ENDPOINT_IN, 0x30, 0, 0, 0, (err, buf) => {
        dev.close();
        setTimeout(resolve, 500);
      });
    } catch (ex) {
      resolve();
    }
  });
}

export async function searchForGuitar(): Promise<Guitar> {
  await jumpToBootloader();
  let board = await findConnectedDevice();
  if (board) {
    const config = await readEeprom(() => {});
    if (config) {
      return {
        newBoard: config.signature != defaultConfig.signature,
        pins: config,
        type: board
      };
    }
  }
  await delay(500)
  return searchForGuitar();
}
