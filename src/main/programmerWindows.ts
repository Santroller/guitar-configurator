

import * as tmp from 'tmp';
import * as usb from 'usb';
import * as fs from 'fs';
import * as path from 'path';
import { BinaryExecution, VID, PID, executeBinary } from './programmer';
import { Guitar, OutputType } from '../common/avr-types';
let infDir = tmp.dirSync();

/**
 * Change drivers for all connected controllers
 */
async function runDevCon(inf: string): BinaryExecution {
    if (process.platform != 'win32') return { code: 0, msg: "" };
    return await new Promise((resolve) => {
        let cb = async function (device: usb.Device) {
            if (device.deviceDescriptor.idVendor == VID && device.deviceDescriptor.idProduct == PID) {
                usb.removeListener('attach', cb);
                resolve(await executeBinary('devcon', ['update', inf, `USB\\VID_${VID.toString(16)}&PID_${PID.toString(16)}`]));
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
        await executeBinary('zadic', ['--vid', `${VID}`, '--pid', `${PID}`, '--usealldevices', '--noprompt'], () => { }, infDir.name);
    }
    //Run devcon so it can change the drivers to the generated one
    //Return code 1 means requires reboot / needs replug
    if ((await runDevCon(inf)).code == 1) {
        //TODO: in this case, tell the user to unplug and replug.
        console.log("Unplug, replug.");
        await new Promise((resolve) => {
            let cb = async function (device: usb.Device) {
                if (device.deviceDescriptor.idVendor == VID && device.deviceDescriptor.idProduct == PID) {
                    usb.removeListener('detach', cb);
                    let cb2 = async function (device: usb.Device) {
                        if (device.deviceDescriptor.idVendor == VID && device.deviceDescriptor.idProduct == PID) {
                            usb.removeListener('attach', cb2);
                            resolve();
                        }
                    }
                    usb.on('attach', cb2);
                }
            }
            usb.on('detach', cb);
        });
    }
}
export async function restoreController(guitar: Guitar) {
    if (process.platform != 'win32') return;
    if (guitar.config.output_type == OutputType.XInput) {
        await runDevCon('c:\\Windows\\INF\\xusb22.inf');
    } else {
        await runDevCon('c:\\Windows\\INF\\input.inf');
    }
}