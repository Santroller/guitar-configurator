// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.
import * as bootstrap from "bootstrap";
import {ipcRenderer} from "electron";
import * as SerialPort from "serialport";

import {getKey, getKeyCode} from "./keymap";
import {pins} from "./pinmap";

let comPort: string = null;
let options: any = {};
function qs(qs : string) {
  return document.querySelector(qs);
}
function qsc(qsl : string, cb : () => void) {
  return qs(qsl).addEventListener("click", cb);
}
function qss(qsl : string) {
  return qs(qsl).classList.remove("hidden");
}
function qsh(qsl : string) {
  return qs(qsl).classList.add("hidden");
}
function addPinTemplate(pin : any) {
  const ele = document.importNode((qs("#input-pin")as any).content, true);
  ele.querySelector("span").innerHTML = pin.name_disp;
  ele.querySelector("select").value = pin.name;
  ele.querySelector("select").innerHTML = Object.entries(pins).map(p => `<option class='cap' value="${p[1]}">${p[0]} (${p[1]})</option>`).join("\n");
  ele.querySelector("select").addEventListener("change", (evt : Event) => {
    options[pin.name] = (evt.target as any).value;
  });
  qs("#pin-bindings").appendChild(ele);
}
function addKeyTemplate(pin : any) {
  const ele = document.importNode((qs("#input-key")as any).content, true);
  ele.querySelector("span").innerHTML = pin.name_disp;
  const sel = ele.querySelector("input");
  sel.id = pin.name;
  sel.value = getKeyCode(pin.value);
  sel.addEventListener("keydown", function (evt : KeyboardEvent) {
    evt = evt || (window.event as KeyboardEvent);
    (evt.target as any).value = evt.code;
    evt.preventDefault();
    options[(evt.target as any).id] = getKey(evt.code);
  });
  qs("#key-bindings").appendChild(ele);
}
window.onload = () => {
  ipcRenderer.send("init");
  qsc("#program", () => {
    ipcRenderer.send("program", comPort);
  });
  qsc("#refresh", () => {
    ipcRenderer.send("list");
  });
  ipcRenderer.on("built", () => {
    qss("#programData");
  });
  ipcRenderer.on("status", (event : Event, status : string) => {
    qs("#status").innerHTML = status;
    qss("#status");
  });
  qsc("#wii", () => {
    options.DEVICE_TYPE = "WII";
    options.F_CPU = 8000000;
    qss("#page2");
    qsh("#page1");
  });
  qsc("#direct", () => {
    options.DEVICE_TYPE = "DIRECT";
    options.F_CPU = 16000000;
    qss("#page2");
    qsh("#page1");
  });
  qsc("#kbd", () => {
    options.OUTPUT_TYPE = "KEYBOARD";
    qss("#page3");
    qsh("#page2");
  });
  qsc("#xin", () => {
    options.OUTPUT_TYPE = "XINPUT";
    qss("#page4");
    qsh("#page2");
  });
  qsc("#bind", () => {
    qss("#page4");
    qsh("#page3");
  });
  qs("#sensitivity").addEventListener("change", (evt : Event) => {
    qs("#sens").innerHTML = (evt.target as any).value;
  });
  qsc("#tilt", () => {
    options.MPU_6050_START = 28000 - (qs("#sensitivity")as any).value * 10;
    if (options.DEVICE_TYPE == "DIRECT") {
      qss("#page5");
    } else {
      qss("#page6");
    }
    qsh("#page4");
    ipcRenderer.send("connect");
    ipcRenderer.send("build", options);
  });
  ipcRenderer.on("list", (event : Event, ports : SerialPort.PortInfo[]) => {
    comPort = ports[0].comName;
    qs("#ports").innerHTML = ports.map(port => `<option value="${port.comName}">${port.comName}</option>`).join("\n");
    qs("#ports").addEventListener("change", (evt : Event) => {
      comPort = (evt.target as any).value;
    });
  });
  ipcRenderer.on("vars", (event : Event, vars : any) => {
    vars.pins.forEach(addPinTemplate.bind(this));
    vars.keys.forEach(addKeyTemplate.bind(this));
  });
};
