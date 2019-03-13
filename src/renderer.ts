// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.
import {ipcRenderer} from "electron";
import * as SerialPort from "serialport";
let comPort: string = null;
window.onload = () => {
  ipcRenderer.send("init");
  document.querySelector("#start").addEventListener("click", () => {
    ipcRenderer.send("connect");
    ipcRenderer.send("build", {DEVICE_TYPE: "DIRECT"});
  });
  document.querySelector("#program").addEventListener("click", () => {
    ipcRenderer.send("program", comPort);
  });
  document.querySelector("#refresh").addEventListener("click", () => {
    ipcRenderer.send("list");
  });
  ipcRenderer.on("built", () => {
    document.querySelector("#programData").classList.remove("hidden");
  });
  ipcRenderer.on("status", (event: Event, status: string) => {
    document.querySelector("#status").innerHTML = status;
  });
  ipcRenderer.on("list", (event: Event, ports: SerialPort.PortInfo[]) => {
    comPort = ports[0].comName;
    document.querySelector("#ports").innerHTML =
        ports
            .map(
                (port) =>
                    `<option value="${port.comName}">${port.comName}</option>`)
            .join("\n");
    (document.querySelector("#ports") as any).onchange = (evt: Event) => {
      comPort = (evt.target as any).value;
    };
  });
};
