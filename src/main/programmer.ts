import * as axios from 'axios';
import * as delay from 'delay';
import * as fs from 'fs';
import * as path from 'path';
import {spawn} from 'pty.js';
import * as rp from 'request-promise-native';
import * as tmp from 'tmp';
import * as usb from 'usb';

import {Board, DeviceType, EepromConfig, Guitar, MemoryLocation, ProgressCallback} from '../common/avr-types';

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
    args: string[], board: Board, progress: ProgressCallback): Promise<void> {
  return new Promise(async (resolve, reject) => {
    // If no board is passed in, then we generate one based on what is plugged
    // in.
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

    let loc = 0;
    proc.stdout.on('data', function(chunk: string) {
      if (chunk.indexOf('writing') != -1 && loc != 0) {
        loc += 100/args.length;
      }
      if (chunk.indexOf('reading on-chip') != -1) {
        loc += 100/args.length;
      }
      let data = /(Writing|Reading) \| #+\s+\| (\d+)% (\d.\d+s)/g.exec(chunk);
      if (data) {
        let p = parseInt(data[2]);
        p /= args.length;
        progress(p + loc, 'avrdude');
      }
    });
  });
}
function avrdudeMemoryArgs(
    location: MemoryLocation, action: MemoryAction, file: string) {
  return ['-U', `${location}:${action}:${file}`];
}
export async function readEeprom(
    progress: ProgressCallback, board: Board): Promise<EepromConfig> {
  const file = tmp.fileSync();
  await spawnAvrDude(
      avrdudeMemoryArgs(MemoryLocation.EEPROM, MemoryAction.READ, file.name),
      board, progress);
  return readData(fs.readFileSync(file.name));
}
async function retrieveHex(file: string, progress: ProgressCallback) {
  let release = await rp({
    url: 'https://api.github.com/repos/sanjay900/Ardwiino/releases/latest',
    json: true,
    headers: {'User-Agent': 'Node'}
  });
  const hex_url =
      release.assets.filter((s: any) => s.name == file)[0].browser_download_url;
  let stream = await axios.default({url: hex_url, responseType: 'stream'});
  let total = stream.headers['content-length'];
  let length = 0;
  stream.data.on('data', (chunk: Buffer) => {
    length += chunk.length;
    progress(length / total * 100, 'download');
  });
  return stream.data;
}
// If we pass in a frequency of zero, we are ignoring the freq parameter.
export async function program(
    device: string, freq: number, config: EepromConfig,
    progress: ProgressCallback) {
  const flash_data = await retrieveHex(
      `ardwiino-${device}${- freq || ''}.hex`, (p, s) => progress(p / 2, s));
  const file_flash = tmp.fileSync();
  const stream_flash = fs.createWriteStream(null!, {fd: file_flash.fd});
  await flash_data.pipe(stream_flash);
  let args = avrdudeMemoryArgs(
      MemoryLocation.FLASH, MemoryAction.WRITE, file_flash.name);
  if (device != "uno-main") {
    const file_eep = tmp.fileSync();
    const stream_eep = fs.createWriteStream(null!, {fd: file_eep.fd});
    stream_eep.write(generateEEP(config));
    args = avrdudeMemoryArgs(
               MemoryLocation.EEPROM, MemoryAction.WRITE, file_eep.name)
               .concat(args);
  }
  boards[device].com = (await searchForACM()).com;
  await spawnAvrDude(args, boards[device], (p, s) => progress(p / 2 + 50, s));
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

export function detectType(config: EepromConfig): DeviceType {
  for (let signature in DeviceType) {
    if (config.signature == parseInt(signature)) {
      return parseInt(signature);
    }
  }
  return DeviceType.Guitar;
}
export async function searchForACM(): Promise<Board> {
  await jumpToBootloader();
  let board = await findConnectedDevice();
  if (board) {
    return board;
  }
  await delay(500);
  return searchForACM();
}
export async function searchForGuitar(): Promise<Guitar> {
  await jumpToBootloader();
  let board = await findConnectedDevice();
  if (board) {
    let config = await readEeprom(() => {}, board);
    let type = detectType(config);
    if (type == DeviceType.Unprogrammed) {
      config = defaultConfig;
    }
    if (config) {
      return {type, config, board};
    }
  }
  await delay(500);
  return searchForGuitar();
}
