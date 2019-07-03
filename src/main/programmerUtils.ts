import { spawn, IPty } from 'node-pty';
import * as path from 'path';
import * as usb from 'usb';

import { PID, VID } from './programmer';
import { swapToWinUSB } from './programmerWindows';

export type BinaryExecution = Promise<{ code: number, msg: string }>;
declare const __static: string;

/**
 * Find a binary under __static
 * @param args the location of the binary
 */
export function findBinary(...args: string[]) {
  const arch = process.platform + '-' + process.arch;
  return path.join(__static, 'binaries', arch, ...args);
}

/**
 * Execute a binary from __static/binaries/arch
 * @param binary the binary to execute
 * @param args arguments to pass to the binary
 * @param onData a function to process stdout from the process
 * @param workingDir the working directory for this process, optional
 * @return The exit code, and the full stdout are returned when the process is
 *     terminated.
 */
export function executeBinary(
  binary: string, args: string[], onData?: (chunk: string, proc: IPty) => void,
  workingDir?: string): BinaryExecution {
  return new Promise(async (resolve) => {
    const extension = process.platform == 'win32' ? '.exe' : '';
    let cwd = workingDir && { cwd: workingDir } || {};
    let proc = spawn(findBinary(binary) + extension, args, cwd);
    let msg = '';
    proc.on('data', (chunk) => {
      onData && onData(chunk, proc);
      msg += chunk + '\n';
    });
    proc.on('exit', exitCode => resolve({ code: exitCode, msg }));
  });
}


export function controlTransfer(
  id: number, direction: number,
  lengthOrData: Buffer | number): Promise<Buffer> {
  return new Promise(async (resolve) => {
    try {
      let dev = usb.findByIds(VID, PID);
      if (!dev) {
        resolve();
        return;
      }
      dev.open();
      dev.controlTransfer(direction, id, 0, 0, lengthOrData, (err, buf) => {
        try {
          dev.close()
        } catch (ex) { }
        resolve(buf);
      });
    } catch (ex) {
      // On windows, we need to install libusb drivers, so if it fails to open,
      // try installing the libusb drivers
      if (process.platform === 'win32') {
        await swapToWinUSB();
        resolve(await controlTransfer(id, direction, lengthOrData));
        return;
      }
      resolve();
    }
  });
}