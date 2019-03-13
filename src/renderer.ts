// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.
import {ipcRenderer} from "electron";
import * as SerialPort from "serialport";
import * as bootstrap from "bootstrap";
let comPort: string = null;
let options: any = {};
function qs(qs: string) {
  return document.querySelector(qs);
}
function qsc(qsl: string, cb: ()=>void) {
  return qs(qsl).addEventListener("click", cb);
}
function qss(qsl: string) {
  return qs(qsl).classList.remove("hidden");
}
function qsh(qsl: string) {
  return qs(qsl).classList.add("hidden");
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
  ipcRenderer.on("status", (event: Event, status: string) => {
    qs("#status").innerHTML = status;
    qss("#status");
  });
  qsc("#wii", ()=>{
    options.DEVICE_TYPE = "WII";
    qss("#page2");
    qsh("#page1");
  });
  qsc("#direct", ()=>{
    options.DEVICE_TYPE = "DIRECT";
    qss("#page2");
    qsh("#page1");
  });
  qsc("#kbd", ()=>{
    options.OUTPUT_TYPE = "KEYBOARD";
    qss("#page3");
    qsh("#page2");
  });
  qsc("#xin", ()=>{
    options.OUTPUT_TYPE = "XINPUT";
    qss("#page4");
    qsh("#page2");
  });
  qsc("#bind", ()=>{
    qss("#page4");
    qsh("#page3");
  });
  qs('#sensitivity').addEventListener('change', (evt: Event) => {
    qs('#sens').innerHTML = (evt.target as any).value;
  })
  qsc("#tilt", ()=>{
    options.MPU_6050_START = 28000 - ((qs('#sensitivity') as any).value * 10);
    qss("#page5");
    qsh("#page4");
    ipcRenderer.send("connect");
    ipcRenderer.send("build", options);
  });
  ipcRenderer.on("list", (event: Event, ports: SerialPort.PortInfo[]) => {
    comPort = ports[0].comName;
    qs("#ports").innerHTML =
        ports
            .map(
                (port) =>
                    `<option value="${port.comName}">${port.comName}</option>`)
            .join("\n");
    (qs("#ports") as any).onchange = (evt: Event) => {
      comPort = (evt.target as any).value;
    };
  });
};