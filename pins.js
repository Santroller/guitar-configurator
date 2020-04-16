.pragma library

function generatePins(count, x, y, width, startId) {
    var ret = [];
    for (var i = 0; i < count; i++) {
        ret.push({x: x+i*width, y:y, id:i+startId});
    }
    return ret;
}
var pinLocations = {
    "images/uno.png": {pins: [...generatePins(8,1083, 13, -40, 0), ...generatePins(6,743, 13, -40, 8), ...generatePins(6,873, 770, 40, 14)], r:30},
    "images/micro.png": {pins: [
            ...generatePins(2,348, 5, -31, 0),
            ...generatePins(8,224, 5, -31, 2),
            {x:8, y: 191, id: 10},
            {x:39, y: 191, id: 16},
            {x:72, y: 191, id: 14},
            {x:103, y: 191, id: 15},
            ...generatePins(4, 132, 191, 32, 18)
        ], r:25},
    "images/leonardo.png": {pins: [...generatePins(8,903, 18, -35, 0), ...generatePins(6,597, 27, -35, 8), ...generatePins(6,727, 697, 35, 18)], r:25},
}
var bindings = {
    "images/uno.png": { 14: "A0", 15: "A1", 16: "A2", 17: "A3", 18: "A4", 19: "A5", 255: "Disabled" },
    "images/micro.png": { 0: "RX0", 1: "RX1", 18: "A0", 19: "A1", 20: "A2", 21: "A3", 255: "Disabled" },
    "images/leonardo.png": { 18: "A0", 19: "A1", 20: "A2", 21: "A3", 22: "A4", 23: "A5", 255: "Disabled" },
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

function getUnused(isGuitar, isWii, isLiveGuitar) {
    let unused = [];
    let labels = getLabels(isGuitar, isWii, isLiveGuitar);
    if (labels === defLabels) return unused;
    for (let label in defLabels) {
        if (!(label in labels)) {
            unused.push(label);
        }
    }
    return unused;
}

function getLabels(isGuitar, isWii, isLiveGuitar) {
    if (isWii && isGuitar) return {"r_y": "Tilt Axis"}
    if (isLiveGuitar) return guitarLiveLabels;
    if (isGuitar) return guitarLabels;
    return defLabels;
}
