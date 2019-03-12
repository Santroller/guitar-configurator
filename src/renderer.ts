// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.
import {ipcRenderer} from "electron";

window.onload = function () {
  document.querySelector("#start").addEventListener("click", () => {
    ipcRenderer.send("connect");
    ipcRenderer.send("build", {DEVICE_TYPE: "DIRECT"});
  });
  ipcRenderer.on("built", () => {
    document.querySelector("#programData").classList.remove("hidden");
  });
  ipcRenderer.on("status", (event: Event, status: string) => {
    document.querySelector("#status").innerHTML = status;
  });
  document.querySelector("#program").addEventListener("click", () => {
    ipcRenderer.send("program");
  });
};
