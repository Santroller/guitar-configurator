import * as avr from "chip.avr.avr109";
import Delay from "delay";
import * as create from "jenkins";
import * as rp from "request-promise-native";
import * as SerialPort from "serialport";
import * as util from "util";

let jenkins: create.JenkinsPromisifiedAPI;
let fileToUpload: string;
export function connect() {
  jenkins = create({
    baseUrl: "https://ardwiino.tangentmc.net",
    crumbIssuer: true,
    promisify: true,
  });
}
export async function build(options: {}, status: (status: string) => void,  callback: () => void) {
  status("Starting build");
  const queuedBuild = await jenkins.job.build({
    name: "Ardwiino",
    parameters: {token: "zIa15bDd9lM6SRIdENAU", ...options},
  });
  let queueData = await jenkins.queue.item(queuedBuild);
  while (!queueData.executable) {
    await Delay(100);
    queueData = await jenkins.queue.item(queuedBuild);
  }
  status("Building firmware");
  const log: NodeJS.ReadableStream =
      await jenkins.build.logStream("Ardwiino", queueData.executable.number);

  log.on("error", function(err) {
    console.log("error", err);
  });
  log.on("end", async function() {
    status("Firmware built");
    fileToUpload = await rp({
      uri: `${queueData.executable.url}/artifact/src/micro/bin/Ardwiino.hex`,
      encoding: "ascii",
    });
    callback();
  });
}
export async function program(port: string, status: (status: string) => void) {
  const sp = new SerialPort(port);
  status("initializing");
  const flasher = await util.promisify(avr.init)(sp, {signature: "CATERIN"});
  await util.promisify(flasher.erase)();
  status("initialized");
  await util.promisify(flasher.program)(fileToUpload);
  status("Programmed! Checking upload.");
  await util.promisify(flasher.verify)();
  status("Programming complete!");
}

export function listPorts() {
  return SerialPort.list();
}
