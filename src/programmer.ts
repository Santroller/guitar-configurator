import * as create from "jenkins";
import * as SerialPort from "serialport";
import * as avr from "chip.avr.avr109";
import * as https from "https";
import * as url from "url";
import Delay from "delay";
let jenkins: create.JenkinsAPI;
let fileToUpload: string;
export function connect() {
  jenkins = create({baseUrl: "https://ardwiino.tangentmc.net", crumbIssuer: true});
}
export function build(options : {}, callback : () => void) {
  console.log(options);
  jenkins.job.build({
    name: "Ardwiino",
    parameters: {
      token: "zIa15bDd9lM6SRIdENAU",
      ...options
    }
  }, function (err, data) {
    if (err) 
      throw err;
    jenkins.queue.item(data, async function (err, id) {
      if (err) 
        throw err;
      
      //Wait until the quiet period ends, and then some more time for the project to start
      await Delay(id.timestamp - new Date().getTime());
      await Delay(10000);
      jenkins.queue.item(data, async function (err, item) {
        if (err) 
          throw err;
        var log = jenkins.build.logStream("Ardwiino", item.executable.number);

        log.on("error", function (err) {
          console.log("error", err);
        });

        log.on("end", function () {
          https.get(url.parse(`${item.executable.url}/artifact/src/micro/bin/Ardwiino.hex`), function (res) {
            var data: any = [];

            res.on("data", function (chunk) {
              data.push(chunk);
            }).on("end", function () {
              fileToUpload = Buffer.concat(data).toString("ascii");
              callback();
            });
          });
        });
      });
    });
  });
}
export function program(status : (status : string) => void) {
  let sp = new SerialPort("/dev/ttyACM0");
  status("initializing");
  avr.init(sp, {
    signature: "CATERIN"
  }, (err, flasher) => {
    if (err) {
      throw err;
    }
    flasher.erase(function () {
      status("initialized");

      flasher.program(fileToUpload, function (err) {
        if (err) 
          throw err;
        status("Programmed! Checking upload.");

        flasher.verify(function (err) {
          if (err) {
            throw err;
          }
          flasher.fuseCheck(function (err) {
            if (err) 
              throw err;
            status("Programming complete!");
          });
        });
      });
    });
  });
}
