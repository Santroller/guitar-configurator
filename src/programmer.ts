import * as avr from 'chip.avr.avr109';
import Delay from 'delay';
import * as create from 'jenkins';
import * as rp from 'request-promise-native';
import * as SerialPort from 'serialport';
import * as usb from 'usb';
import * as util from 'util';
import * as xbox from 'xbox-ctrl';

let jenkins: create.JenkinsPromisifiedAPI;
let fileToUpload: string;
export function connect() {
  jenkins = create({
    baseUrl: 'https://ardwiino.tangentmc.net',
    crumbIssuer: true,
    promisify: true
  });
}
const pins_start = 'DEVICE_TYPE == DIRECT\n';
const keys_start = 'OUTPUT_TYPE == KEYBOARD\n';
function parseVariables(data: string, start: string) {
  var section = data.substring(data.indexOf(start) + start.length);
  section = section.substring(0, section.indexOf('#endif'));
  return section.split('#define ').slice(1).map((s2: string) => {
    let split = s2.replace(/\n/g, ' ').split(' ');
    let isPin = split[0].indexOf('PIN_') != -1;
    let cleaned_name = split[0].replace('PIN_', '');
    cleaned_name = cleaned_name.replace('KEY_', '');
    return {
      name: split[0],
      isPin: isPin,
      name_disp: cleaned_name.toLowerCase().replace(/_/g, ' '),
      value: split[1],
      comments: split.slice(2)
                    .join(' ')
                    .replace(/\/\//g, '')
                    .trim()
                    .replace(/_/g, ' ')
    };
  });
}
export async function getVariables() {
  return rp('https://raw.githubusercontent.com/sanjay900/Ardwiino/master/src/config/config.h')
      .then(data => ({
              keys: parseVariables(data, keys_start),
              pins: parseVariables(data, pins_start),
              mpu: data.substring(data.indexOf('MPU_6050_START'))
                       .split(' ')[1]
                       .trim()
            }));
}
export async function build(
    options: {}, status: (status: string) => void, callback: () => void) {
  status('Starting build');
  const queuedBuild = await jenkins.job.build({
    name: 'Ardwiino',
    parameters: {token: 'zIa15bDd9lM6SRIdENAU', ...options}
  });
  let queueData = await jenkins.queue.item(queuedBuild);
  while (!queueData.executable) {
    await Delay(100);
    queueData = await jenkins.queue.item(queuedBuild);
  }
  status('Building firmware');
  const log: NodeJS.ReadableStream =
      await jenkins.build.logStream('Ardwiino', queueData.executable.number);

  log.on('error', function(err) {
    console.log('error', err);
  });
  log.on('end', async function() {
    status('Firmware built');
    fileToUpload = await rp({
      uri: `${queueData.executable.url}/artifact/src/micro/bin/Ardwiino.hex`,
      encoding: 'ascii'
    });
    await bootloader();
    callback();
  });
}
export async function bootloader() {
  if (process.platform === 'win32') {
    let xbox = await import('xbox-ctrl');
    xbox.vibrateAll(1);
  } else {
    return await new Promise((resolve => {
      let dev = usb.findByIds(0x1209, 0x2882);
      dev.open();
      dev.controlTransfer(usb.LIBUSB_ENDPOINT_IN, 0x30, 0, 0, 0, (err, buf) => {
        resolve();
      });
    }))
  }
}
export async function program(port : string, status : (status : string) => void) {
  const sp = new SerialPort(port);
  status('Setting up Programmer');
  const flasher = await util.promisify(avr.init)(sp, {signature: 'CATERIN'});
  await util.promisify(flasher.erase.bind(flasher))();
  status('Programming...');
  await util.promisify(flasher.program.bind(flasher))(fileToUpload);
  status('Programmed! Checking upload.');
  await util.promisify(flasher.verify.bind(flasher))();
  status('Programming complete!');
}

export function listPorts() {
  return SerialPort.list();
}
