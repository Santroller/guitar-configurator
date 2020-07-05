var oldNames = {
    "up": "D-pad Up",
    "down": "D-pad Down",
    "left": "D-pad Left",
    "right": "D-pad Right",
    "start": "Start Button",
    "back": "Back Button",
    "left_stick": "Left Stick Click",
    "right_stick": "Right Stick Click",
    "LB": "Left Bumper",
    "RB": "Right Bumper",
    "home": "Home Button",
    "a": "A Button",
    "b": "B Button",
    "x": "X Button",
    "y": "Y Button",
    "lt": "Left Shoulder Axis",
    "rt": "Right Shoulder Axis",
    "l_x_lt": "Left Joystick X Axis +",
    "l_x_gt": "Left Joystick X Axis -",
    "l_y_lt": "Left Joystick Y Axis +",
    "l_y_gt": "Left Joystick Y Axis -",
    "r_x_lt": "Right Joystick X Axis +",
    "r_x_gt": "Right Joystick X Axis -",
    "r_y_lt": "Right Joystick Y Axis +",
    "r_y_gt": "Right Joystick Y Axis -",
}
var newNames = {
    "Up": "D-pad Up",
    "Down": "D-pad Down",
    "Left": "D-pad Left",
    "Right": "D-pad Right",
    "Start": "Start Button",
    "Back": "Back Button",
    "LeftStick": "Left Stick Click",
    "RightStick": "Right Stick Click",
    "LB": "Left Bumper",
    "RB": "Right Bumper",
    "Home": "Home Button",
    "A": "A Button",
    "B": "B Button",
    "X": "X Button",
    "Y": "Y Button",
    "Lt": "Left Shoulder Axis",
    "Rt": "Right Shoulder Axis",
    "LXPos": "Left Joystick X Axis +",
    "LXNeg": "Left Joystick X Axis -",
    "LYPos": "Left Joystick Y Axis +",
    "LYNeg": "Left Joystick Y Axis -",
    "RXPos": "Right Joystick X Axis +",
    "RXNeg": "Right Joystick X Axis -",
    "RYPos": "Right Joystick Y Axis +",
    "RYNeg": "Right Joystick Y Axis -",
}
let oldN = Object.keys(oldNames).map(s => `g${s}.svg`);
let newN = Object.keys(newNames).map(s => `g${s}.svg`);
const { resolve, basename } = require('path');
const { rename, renameSync, readFileSync, writeFileSync } = require('fs');
const { readdir } = require('fs').promises;

// async function* getFiles(dir) {
//     const dirents = await readdir(dir, { withFileTypes: true });
//     for (const dirent of dirents) {
//         const res = resolve(dir, dirent.name);
//         if (dirent.isDirectory()) {
//             yield* getFiles(res);
//         } else {
//             yield res;
//         }
//     }
// }
// (async () => {
//     for await (const f of getFiles('../images')) {
//         let file = basename(f);
//         if (oldN.includes(file)) {
//             let newF = f.replace(file,newN[oldN.indexOf(file)]);
//             renameSync(f,newF);
//         }

//     }
// })()

// let qrc = readFileSync("../resources.qrc").toString();
// for (let i = 0; i < oldN.length; i++) {
//     qrc = qrc.replace(new RegExp(oldN[i],'g'),newN[i]);
// }
// console.log(qrc);
// writeFileSync("../resources.qrc",qrc);