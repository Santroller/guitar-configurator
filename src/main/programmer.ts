import * as usb from 'usb';
import * as usbDetection from 'usb-detection';
export const VID = 0x1209;
export const PID = 0x2882;

import { DeviceType, EepromConfig, Guitar, Subtype } from '../common/avr-types';

import { readData, EepromConfigLength, writeData, defaultConfig } from './eeprom';
import { searchForProgrammer } from './programmerFirmware';
import { controlTransfer } from './programmerUtils';
import { swapToWinUSB } from './programmerWindows';
import * as xinput from "xinput";
usbDetection.startMonitoring();

export async function readConfig(): Promise<EepromConfig> {
  let data =
    await controlTransfer(0x32, usb.LIBUSB_ENDPOINT_IN, EepromConfigLength);
  return data && readData(data);
}

export function writeConfig(config: EepromConfig) {
  return controlTransfer(0x31, usb.LIBUSB_ENDPOINT_OUT, writeData(config))
}

export function detectType(config: EepromConfig): DeviceType {
  for (let signature in DeviceType) {
    if (config.signature == parseInt(signature)) {
      return parseInt(signature);
    }
  }
  return DeviceType.Unprogrammed;
}
export async function searchForGuitar(cb: (guitar: Guitar) => void) {
  let found = false;
  let init = true;
  let devCb = async function (device: usb.Device) {
    usbDetection.find(device.deviceDescriptor.idVendor, device.deviceDescriptor.idProduct, (err, dev)=>{
      console.log(dev);
      console.log(device);
    });
    // if (device.deviceDescriptor.idVendor == VID &&
    //   device.deviceDescriptor.idProduct == PID) {
    //   usb.removeListener('attach', devCb);
    //   found = true;
    //   if (process.platform == 'win32') {
    //     console.log(device);
    //     //For xinput, we have the ability to work out what kind of device we are using.
    //     let found = false;
    //     [0, 1, 2, 3].forEach(id => {
    //       if (xinput.IsConnected(id)) {
    //         let config = defaultConfig;
    //         config.subtype = xinput.GetCapabilities(id).subType;
    //         cb({ type: DeviceType.Unknown, config, updating: false });
    //         found = true;
    //       }
    //     })
    //     if (!found) {
    //       let config = defaultConfig;
    //       config.subtype = Subtype.Gamepad;
    //       cb({ type: DeviceType.Unknown, config, updating: false });
    //     }
    //   } else {
    //     // await swapToWinUSB();
    //     let config = await readConfig();
    //     let type = detectType(config);
    //     cb({ type, config, updating: false });
    //   }
    // } else if (!init) {
    //   setTimeout(async () => {
    //     let unprogrammed = await searchForProgrammer();
    //     if (unprogrammed) {
    //       cb(unprogrammed);
    //     }
    //   }, 500);
    // }
  };
  usb.getDeviceList().forEach(devCb);
  if (found) return;
  init = false;
  let unprogrammed = await searchForProgrammer();
  if (unprogrammed) {
    cb(unprogrammed);
  }
  usb.on('attach', devCb);
}
