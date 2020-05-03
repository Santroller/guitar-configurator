.pragma library

function generatePins(count, x, y, width, startId) {
    var ret = [];
    for (var i = 0; i < count; i++) {
        ret.push({x: x+i*width, y:y, id:i+startId});
    }
    return ret;
}
var pinLocations = {
    "images/ArduinoUno.svg": {width: 284, height:208, pins: [...generatePins(8,254, 16, -9, 0), ...generatePins(6,177, 16, -9, 8), ...generatePins(6,209, 186, 9, 14)], r:4},
    "images/ArduinoProMicro.svg": {width: 262, height:138, pins: [
            ...generatePins(2,216.75, 5, -19, 0),
            ...generatePins(8,140.25, 5, -19, 2),
            {x:7, y: 121, id: 10},
            {x:26.25, y: 121, id: 16},
            {x:45.25, y: 121, id: 14},
            {x:64.25, y: 121, id: 15},
            ...generatePins(4, 83.25, 121, 19, 18)
        ], r:11.5},
    "images/ArduinoLeonardo.svg": {width: 199, height:150, pins: [...generatePins(8,183, 5, -7, 0), ...generatePins(6,121, 5, -7, 8), ...generatePins(6,146, 141, 7, 18)], r:4},
}
var bindings = {
    "images/ArduinoUno.svg": { 14: "A0", 15: "A1", 16: "A2", 17: "A3", 18: "A4", 19: "A5", 255: "Disabled" },
    "images/ArduinoProMicro.svg": { 0: "RX0", 1: "RX1", 18: "A0", 19: "A1", 20: "A2", 21: "A3", 255: "Disabled" },
    "images/ArduinoLeonardo.svg": { 18: "A0", 19: "A1", 20: "A2", 21: "A3", 22: "A4", 23: "A5", 255: "Disabled" },
}
var guitarLabels = {
    "up": "Strum Up",
    "down": "Strum Down",
    "left": "DPad Left",
    "right": "DPad Right",
    "start": "Start Button",
    "back": "Select Button",
    "home": "Home Button",
    "a": "Green Fret",
    "b": "Red Fret",
    "y": "Yellow Fret",
    "x": "Blue Fret",
    "LB": "Orange Fret",
    "l_x": "Joystick X Axis",
    "l_y": "Joystick Y Axis",
    "r_x": "Whammy",
    "r_y": "Tilt Axis",
}

var guitarRBLabels = {
    "up": "Strum Up",
    "down": "Strum Down",
    "left": "DPad Left",
    "right": "DPad Right",
    "start": "Start Button",
    "back": "Select Button",
    "home": "Home Button",
    "a": "Green Fret",
    "b": "Red Fret",
    "y": "Yellow Fret",
    "x": "Blue Fret",
    "LB": "Orange Fret",
    "lt": "Effects Switch",
    "l_x": "Joystick X Axis",
    "l_y": "Joystick Y Axis",
    "r_x": "Whammy",
    "r_y": "Tilt",
}

var guitarLiveLabels = {
    "up": "Strum Up",
    "down": "Strum Down",
    "left": "DPad Left",
    "right": "DPad Right",
    "start": "Start Button",
    "back": "Select Button",
    "home": "Home Button",
    "a": "Black 1",
    "b": "Black 2",
    "y": "Black 3",
    "x": "White 1",
    "LB": "White 2",
    "RB": "White 3",
    "l_x": "Joystick X Axis",
    "l_y": "Joystick Y Axis",
    "r_x": "Whammy",
    "r_y": "Tilt Axis",
}

var defLabels = {
    "up": "DPad Up",
    "down": "DPad Down",
    "left": "DPad Left",
    "right": "DPad Right",
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
    "l_x": "Left Joystick X Axis",
    "l_y": "Left Joystick Y Axis",
    "r_x": "Right Joystick X Axis",
    "r_y": "Right Joystick Y Axis",
}

function getUnused(isGuitar, isWii, isLiveGuitar, isRB) {
    let unused = [];
    let labels = getLabels(isGuitar, isWii, isLiveGuitar, isRB);
    if (labels === defLabels) return unused;
    for (let label in defLabels) {
        if (!(label in labels)) {
            unused.push(label);
        }
    }
    return unused;
}

function getLabels(isGuitar, isWii, isLiveGuitar, isRB) {
    if (isWii && isGuitar) return {"r_y": "Tilt Axis"}
    if (isRB) return guitarRBLabels;
    if (isLiveGuitar) return guitarLiveLabels;
    if (isGuitar) return guitarLabels;
    return defLabels;
}
