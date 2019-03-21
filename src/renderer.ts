// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.
import * as bootstrap from 'bootstrap';
import {ipcRenderer} from 'electron';
import * as SerialPort from 'serialport';
import * as $ from 'jquery';

import {getKey, getKeyCode} from './keymap';
import {pins} from './pinmap';

let comPort: string = null;
let options: any = {};
let currentPage: string = '#page1';
let lastPage: string[] = [];
let defaultMPU: number;
function qs(qs: string, ele: Element|Document = document) {
  return ele.querySelector(qs);
}
function bindClick(
    qsl: string, cb: () => void, ele: Element|Document = document) {
  return qs(qsl, ele).addEventListener('click', cb);
}
function selectAndShow(qsl: string, ele: Element|Document = document) {
  return qs(qsl, ele).classList.remove('hidden');
}
function selectAndHide(qsl: string, ele: Element|Document = document) {
  return qs(qsl, ele).classList.add('hidden');
}
function makeOption(value: string, label: string) {
  return `<option value="${value}">${label}</option>`;
}
function showPage(next: string) {
  lastPage.push(currentPage);
  selectAndHide(currentPage);
  selectAndShow(next);
  currentPage = next;
}
function bindNextPage(button: string, next: string, cb: () => void = () => {}) {
  bindClick(button, () => {
    cb();
    showPage(next);
    if (next == '#upload') {
      ipcRenderer.send('connect');
      ipcRenderer.send('build', options);
    }
  });
}
function addPinTemplate(pin: any, isJoy: boolean) {
  if (pin.isPin) {
    const ele = document.importNode((qs('#input-pin') as any).content, true);
    qs('span', ele).innerHTML = pin.name_disp;
    qs('select', ele).innerHTML =
        Object.entries(pins)
            .map(p => makeOption(p[1] + '', `${p[0]} (${p[1]})`))
            .join('\n');
    (qs('select', ele) as HTMLInputElement).value = pin.value;
    qs('select', ele).addEventListener('change', (evt: Event) => {
      options['PIN_' + pin.name] = (evt.target as HTMLInputElement).value;
    });
    if (pin.name.indexOf('JOY') != -1) {
      qs('div', ele).classList.add('joy');
      if (!isJoy) {
        selectAndHide('div', ele);
      }
    } else if (
        pin.name.indexOf('RIGHT') != -1 || pin.name.indexOf('LEFT') != -1) {
      qs('div', ele).classList.add('dpad');
      if (isJoy) {
        selectAndHide('div', ele);
      }
    }
    qs('#pin-bindings').appendChild(ele);
  }
}
function addKeyTemplate(pin: any) {
  const ele = document.importNode((qs('#input-key') as any).content, true);
  qs('span', ele).innerHTML = pin.name_disp;
  const sel = qs('input', ele) as HTMLInputElement;
  sel.id = pin.name;
  sel.value = getKeyCode(pin.value);
  sel.addEventListener('keydown', function(evt: KeyboardEvent) {
    let target = evt.target as HTMLInputElement;
    target.value = evt.code;
    evt.preventDefault();
    options['KEY_' + target.id] = getKey(evt.code);
  });
  qs('#key-bindings').appendChild(ele);
}
function bindPages() {
  bindNextPage('#wii', '#page2', () => {
    options.DEVICE_TYPE = 'WII';
    options.F_CPU = 8000000;
  });
  bindNextPage('#direct', '#page2', () => {
    options.DEVICE_TYPE = 'DIRECT';
    options.F_CPU = 16000000;
  });
  bindNextPage('#kbd', '#page3', () => {
    options.OUTPUT_TYPE = 'KEYBOARD';
  });
  bindNextPage('#xin', '#page4', () => {
    options.OUTPUT_TYPE = 'XINPUT';
  });
  bindNextPage('#bind', '#page4');
  bindNextPage('#pinbind', '#upload');
  bindClick('#tilt', () => {
    if (options.DEVICE_TYPE == 'DIRECT') {
      showPage('#page5');
    } else {
      showPage('#upload');
    }
  });
  Array.from(document.querySelectorAll('.back')).forEach(q => {
    q.addEventListener('click', () => {
      selectAndHide(currentPage);
      selectAndShow((currentPage = lastPage.pop()));
    });
  });
}
function bindResponses() {
  ipcRenderer.on('built', () => {
    selectAndShow('#programData');
  });
  let progress = -50;
  ipcRenderer.on('status', (event: Event, status: string) => {
    const ste = $('#status');
    ste.html(status);
    progress += 50;
    if (progress > 100) progress = 0;
    ste.css('width', progress + '%');
  });
  ipcRenderer.on('error', (event: Event, status: string) => {
    qs('#status').innerHTML = status;
    qs('#status').classList.add('bg-danger');
  });

  ipcRenderer.on('list', (event: Event, ports: SerialPort.PortInfo[]) => {
    comPort = ports[0].comName;
    qs('#ports').innerHTML =
        ports.map(port => makeOption(port.comName, port.comName)).join('\n');
    qs('#ports').addEventListener('change', (evt: Event) => {
      comPort = (evt.target as any).value;
    });
  });
  ipcRenderer.on('vars', (event: Event, vars: any) => {
    defaultMPU = vars.mpu;
    vars.pins.forEach(
        addPinTemplate.bind(this), vars.pins.DIRECTION_MODE == 'JOY');
    vars.keys.forEach(addKeyTemplate.bind(this));
    qs('#DIRECTION_MODE select').addEventListener('change', (evt: Event) => {
      options['DIRECTION_MODE'] = (evt.target as HTMLInputElement).value;
      const isJoy = options['DIRECTION_MODE'] == 'JOY';
      Array.from(document.querySelectorAll(isJoy ? '.joy' : '.dpad'))
          .forEach(s => s.classList.remove('hidden'));
      Array.from(document.querySelectorAll(isJoy ? '.dpad' : '.joy'))
          .forEach(s => s.classList.add('hidden'));
    });
  });
}
window.onload = () => {
  bindPages();
  bindResponses();
  bindClick('#program', () => {
    ipcRenderer.send('program', comPort);
  });
  bindClick('#refresh', () => {
    ipcRenderer.send('list');
  });
  qs('#sensitivity').addEventListener('change', (evt: Event) => {
    const target = evt.target as HTMLInputElement;
    qs('#sens').innerHTML = target.value;
    options.MPU_6050_START = defaultMPU - parseInt(target.value) * 10;
  });
  ipcRenderer.send('init');
};
