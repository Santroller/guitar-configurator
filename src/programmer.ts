import * as avr from "chip.avr.avr109";
import Delay from "delay";
import * as SerialPort from "serialport";
import * as usb from "usb";

import {EepromConfig, readData, writeData} from "./eeprom";
export type Guitar = {
  found: boolean;
  dfu: boolean;
  pins: EepromConfig;
};
export async function program(port : string, status : (status : string) => void) {
  // TODO once we have a build server set up that pushes releases, then replace
  // TODO if we go this route, we need to save some sort of protocol version so that updates to the config don't cause problems.
  // TODO also, check if this requires writing back the eeprom or not.
  // TODO also, use readFreq in cases where we have that ability.
  // this with something that just grabs the latest firmware and uploads.
  // const sp = new SerialPort(port);
  // status("Setting up Programmer");
  // const flasher = await util.promisify(avr.init)(sp, {signature: "CATERIN"});
  // await util.promisify(flasher.erase.bind(flasher))();
  // status("Programming...");
  // await util.promisify(flasher.program.bind(flasher))(fileToUpload);
  // status("Programmed! Checking upload.");
  // await util.promisify(flasher.verify.bind(flasher))();
  // status("Programming complete!");
  // await sp.close();
}

export async function readFreq(): Promise<number> {
  let device = await findDevice();
  return await new Promise((resolve, reject) => {
    const sp = new SerialPort(device.comName);
    sp.on("data", function (data) {
      sp.close();
      resolve(parseInt(data + ""));
    });
    sp.write("f", function (err) {
      if (err) {
        sp.close();
        reject(err);
      }
    });
  });
}

export async function read(): Promise<EepromConfig> {
  let device = await findDevice();
  return await new Promise((resolve, reject) => {
    const sp = new SerialPort(device.comName);
    sp.on("data", function (data) {
      sp.close();
      resolve(readData(data));
    });
    sp.write("r", function (err) {
      if (err) {
        sp.close();
        reject(err);
      }
    });
  });
}

export async function write(data : EepromConfig) {
  let device = await findDevice();
  return await new Promise((resolve, reject) => {
    const sp = new SerialPort(device.comName);
    sp.write([
      "w".charCodeAt(0), ...writeData(data)
    ], function (err) {
      sp.close();
      if (err) {
        reject(err);
      } else {
        resolve();
      }
    });
  });
}

function jumpToProgrammer() {
  return new Promise((resolve, reject) => {
    let dev = usb.findByIds(0x1209, 0x2882);
    dev.open();
    dev.controlTransfer(usb.LIBUSB_ENDPOINT_IN, 0x31, 0, 0, 0, (err, buf) => {
      dev.close();
      resolve();
    });
  });
}

export async function jumpToMain() {
  let device = await findDevice();
  return await new Promise((resolve, reject) => {
    const sp = new SerialPort(device.comName);
    sp.write("b", function (err) {
      sp.close();
      if (err) {
        reject(err);
      } else {
        resolve();
      }
    });
  });
}

export async function findDevice() {
  return (await SerialPort.list()).filter(s => s.vendorId && s.vendorId == "1209" && s.productId && s.productId == "2882")[0];
}

export async function findDFUDevice() {
  return (await SerialPort.list()).filter(s => s.vendorId && (s.vendorId == "2341" || s.vendorId == "1b4f" || s.vendorId == "2a03"))[0];
}

export function searchForGuitar(): Promise<Guitar> {
  var jumped = false;
  return new Promise(resolve => {
    const interval = setInterval(async () => {
      if (await findDevice()) {
        resolve({found: true, dfu: false, pins: await read()});
        clearInterval(interval);
      } else if (await findDFUDevice()) {
        resolve({found: true, dfu: true, pins: null});
        clearInterval(interval);
      } else if (usb.findByIds(0x1209, 0x2882) && process.platform !== "win32" && !jumped) {
        jumpToProgrammer();
        jumped = true;
      }
    }, 500);
  });
}
