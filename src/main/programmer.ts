import * as delay from 'delay';
import * as fs from 'fs';
import {spawn} from 'node-pty';
import * as path from 'path';
import * as tmp from 'tmp';
import * as usb from 'usb';
declare const __static: string;
const VID = 0x1209;
const PID = 0x2882;

import {Board, DeviceType, EepromConfig, Guitar, ProgressCallback, OutputType} from '../common/avr-types';

import {boards, findConnectedDevice, getAvrdudeArgs} from './boards';
import {defaultConfig, generateEEP, readData} from './eeprom';


let infDir = tmp.dirSync();
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
function getExtension() {
  if (process.platform == 'win32') {
    return '.exe';
  }
  return '';
}
function findBinary(...args: string[]) {
  return path.join(
      __static, 'binaries', process.platform + '-' + process.arch, ...args);
}
function getProgress(args: string[], progress: ProgressCallback) {
  let loc = 0;
  return (chunk: string) => {
    if (chunk.indexOf('writing') != -1 && loc != 0) {
      loc += 100 / args.length;
    }
    if (chunk.indexOf('reading on-chip') != -1) {
      loc += 100 / args.length;
    }
    let data = /(Writing|Reading) \| #+\s+\| (\d+)% (\d.\d+s)/g.exec(chunk);
    if (data) {
      let p = parseInt(data[2]);
      p /= args.length;
      progress(p + loc, 'avrdude');
    }
  }
}
function spawnAvrDude(
    args: string[], board: Board,
    chunkProcessor: (chunk: string) => void): Promise<void> {
  return new Promise(async (resolve) => {
    const avrdudePath = findBinary('avrdude');
    let proc = spawn(
        avrdudePath + getExtension(),
        [...getAvrdudeArgs(board), ...args], {});
    let msg = "";
    proc.on('data', (chunk)=>{
      chunkProcessor(chunk);
      msg += chunk + "\n";
    });  
    proc.on('exit', function(exitCode: number) {
      if (exitCode != 0) {
        console.log(`Avrdude exited with ${exitCode}, output: ${msg}`);
      }
      resolve();
    });
  });
}
function avrdudeMemoryArgs(
    location: MemoryLocation, action: MemoryAction, file: string) {
  if (action == MemoryAction.READ) {
    return ['-U', `${location}:${action}:${file}:${AvrFileFormat.RawBinary}`];
  } else {
    return ['-U', `${location}:${action}:${file}:${AvrFileFormat.AutoDetect}`];
  }
}
export async function readEeprom(
    progress: ProgressCallback, board: Board): Promise<EepromConfig> {
  const file = tmp.fileSync();
  let args =
      avrdudeMemoryArgs(MemoryLocation.EEPROM, MemoryAction.READ, file.name);
  await spawnAvrDude(args, board, getProgress(args, progress));
  return readData(fs.readFileSync(file.name));
}
// If we pass in a frequency of zero, we are ignoring the freq parameter.
export async function program(
    device: string, guitar: Guitar, progress: ProgressCallback) {
  let args: string[] = [];
  if (guitar.updating) {
    let file_flash = path.join(
        __static, 'firmwares', 'ardwiino',
        `ardwiino-${guitar.board.name}-${guitar.board.processor}-${
            guitar.config.cpu_freq}.hex`);
    args =
        avrdudeMemoryArgs(MemoryLocation.FLASH, MemoryAction.WRITE, file_flash)
            .concat(args);
  }
  // In dual cpu arduinos, we only need to flash the eeprom of the first chip
  if (!device.endsWith('-main')) {
    const file_eep = tmp.fileSync();
    const stream_eep = fs.createWriteStream(null!, {fd: file_eep.fd});
    stream_eep.write(generateEEP(guitar.config));
    args = avrdudeMemoryArgs(
               MemoryLocation.EEPROM, MemoryAction.WRITE, file_eep.name)
               .concat(args);
  }
  if (args.length == 0) return;
  guitar.board = boards[device];
  guitar.board.com = (await findAndJumpBootloader()).com;
  await spawnAvrDude(
      args, boards[device],
      getProgress(args, (p, s) => progress(p / 2 + 50, s)));
      
  await restoreController(guitar);
    
}

export async function getConnectedBoard(board: Board) {
  let partId;
  // Avrdude will take a guess at which part is connected. Record that guess
  await spawnAvrDude([], board, chunk => {
    if (chunk.includes('probably')) {
      partId = chunk.split('probably ')[1].trim().slice(0, -1);
    }
    //Also record if we have a uno with the hoodloader bootloader.
    if (chunk.includes('HL2.0.5')) {
      board.hasBootloader = true;
    }
  });
  // The guess however is only a part id, so we need to resolve it back to the
  // parts real name.
  let avrConfig = fs.readFileSync(findBinary('avrdude.conf'), 'utf8');
  let part =
      new RegExp(partId + '";[\\s\\S]\\s+desc\\s+= "([^"]+)').exec(avrConfig);
  if (!part) {
    return board;
  }
  board.processor = part && part[1].toLowerCase();
  return board;
}

export async function programHoodloader(
    guitar: Guitar, progress: ProgressCallback) {
  let args: string[] = [];
  let file_flash = path.join(
      __static, 'firmwares', 'hoodloader',
      `hoodloader-${guitar.board.name}.hex`);
  args = avrdudeMemoryArgs(MemoryLocation.FLASH, MemoryAction.WRITE, file_flash)
             .concat(args);
  await spawnAvrDude(
      args, guitar.board, getProgress(args, (p, s) => progress(p, s)));
}

/**
 * Change drivers for all connected controllers
 */
export function runDevCon(inf: string) {
  if (process.platform != 'win32') return;
  return new Promise(async (resolve) => {
    let cb = function (device: usb.Device) {
      usb.removeListener('attach', cb);
      if (device.deviceDescriptor.idVendor == VID && device.deviceDescriptor.idProduct == PID) {
        const devConPath = findBinary('devcon.exe');
        let proc = spawn(devConPath, ['update', inf, `USB\\VID_${VID.toString(16)}&PID_${PID.toString(16)}`], {});
        proc.on('exit', resolve);
      }
    }
    let dev = usb.findByIds(VID, PID);
    if (dev) {
      cb(dev);
    } else {
      usb.on('attach', cb)
    }
  });
}
export async function swapToWinUSB() {
  if (process.platform != 'win32') return;
  //Zadic will create a libusb inf file for us, and register certificitates to the local machine
  let inf = path.join(infDir.name, 'usb_driver', 'libusb_device.inf');
  //Zadic inf is missing, create it
  if (!fs.existsSync(inf)) {
    //Call zadic
    await new Promise(async (resolve) => {
      const zadicPath = findBinary('zadic.exe');
      let proc = spawn(zadicPath, ['--vid', `${VID}`, '--pid', `${PID}`, '--usealldevices', '--noprompt'], {cwd: infDir.name});
      proc.on('exit', resolve);
    });
  }
  //Run devcon so it can change the drivers to the generated one
  await runDevCon(inf);
}
export async function restoreController(guitar: Guitar) {
  if (process.platform != 'win32') return;
  if (guitar.config.output_type == OutputType.XInput) {
    await runDevCon('c:\\Windows\\INF\\xusb22.inf');
  } else {
    await runDevCon('c:\\Windows\\INF\\input.inf');
  }
}
export async function jumpToBootloader() {
  await new Promise(async (resolve) => {
    try {
      let dev = usb.findByIds(VID, PID);
      if (!dev) {
        resolve();
        return;
      }
      dev.open();
      // The ardwiino firmware responds to a control transfer of 0x30 by jumping
      // to the bootloader.
      dev.controlTransfer(usb.LIBUSB_ENDPOINT_IN, 0x30, 0, 0, 0, (err, buf) => {
        dev.close();
        setTimeout(resolve, 500);
      });
    } catch (ex) {
      //On windows, we need to install libusb drivers, so if it fails to open, try installing the libusb drivers
      if (process.platform === 'win32') {
        await swapToWinUSB(); 
      }
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
  return DeviceType.Unprogrammed;
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
export async function searchForGuitar(): Promise<Guitar> {
  await jumpToBootloader();
  let board = await findConnectedDevice();
  if (board) {
    let config = defaultConfig;
    // For the uno (and anything else with two processors) the config is always
    // stored on the usb portion
    if (board.name.endsWith('-main')) {
      let board2 = boards[board.name.split('-')[0] + '-usb'];
      board2.com = board.com;
      //Get avrdude to fill in information we can't easily guess
      board = await getConnectedBoard(board2);
      await findAndJumpBootloader();
    }
    let updating = false;
    let type = DeviceType.Unprogrammed;
    if (board.hasBootloader) {
      config = await readEeprom(() => {}, board);
      type = detectType(config);
      updating = type == DeviceType.Unprogrammed;
      if (updating) {
        config = defaultConfig;
        // The uno is always 16000000
        if (board.name.indexOf('uno') != -1) {
          config.cpu_freq = 16000000;
        }
      }
    }
    return {type, config, board, updating};
  }
  await delay(500);
  return searchForGuitar();
}
