import * as usb from 'usb';
export const VID = 0x1209;
export const PID = 0x2882;

import {DeviceType, EepromConfig, Guitar} from '../common/avr-types';

import {readData, EepromConfigLength, writeData} from './eeprom';
import {searchForProgrammer} from './programmerFirmware';
import {controlTransfer} from './programmerUtils';


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
  let devCb = async function(device: usb.Device) {
    if (device.deviceDescriptor.idVendor == VID &&
        device.deviceDescriptor.idProduct == PID) {
      usb.removeListener('attach', devCb);
      found = true;
      let config = await readConfig();
      let type = detectType(config);
      cb({type, config, updating: false});
    } else if (!init) {
      setTimeout(async () => {
        let unprogrammed = await searchForProgrammer();
        if (unprogrammed) {
          cb(unprogrammed);
        }
      }, 500);
    }
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
