.pragma library

function generatePins(count, x, y, width, startId, skipped) {
    var ret = [];
    for (var i = 0; i < count; i++) {
        if (skipped && skipped.includes(i + startId)) x += width;
        ret.push({ x: x, y: y, id: i + startId });
        x += width;
    }
    return ret;
}
var pinLocations = {
    "/images/ArduinoMega.svg": {
        width: 408, height: 198, pins: [
            ...generatePins(8, 263.5, 5.5, -9.5, 0),
            ...generatePins(6, 177.5, 5.5, -9.5, 8),
            ...generatePins(8, 283, 5.5, 9.5, 14),
            ...generatePins(8, 215.75, 188, 9.5, 54),
            ...generatePins(8, 302, 188, 9.5, 62),
        ],
        r: 4
    },
    "/images/ArduinoMegaADK.svg": {
        width: 306, height: 151, pins: [
            ...generatePins(8, 196.6, 5.82, -7.2, 0),
            ...generatePins(6, 134.5, 5.82, -7.2, 8),
            ...generatePins(8, 211, 5.82, 7.2, 14),
            ...generatePins(8, 160.5, 142.5, 7.2, 54),
            ...generatePins(8, 225.3, 142.5, 7.2, 62),
        ],
        r: 4
    },
    "/images/ArduinoUno.svg": {
        width: 284, height: 208, pins: [
            ...generatePins(8, 254, 16, -9, 0),
            ...generatePins(6, 177, 16, -9, 8),
            ...generatePins(6, 209, 186, 9, 14)],
        r: 4
    },
    "/images/ArduinoProMicro.svg": {
        width: 138, height: 68, pins: [
            ...generatePins(2, 109, 3, -9.5, 0),
            ...generatePins(8, 70.5, 3, -9.6, 2),
            { x: 3, y: 61, id: 10 },
            { x: 12.75, y: 61, id: 16 },
            { x: 22.25, y: 61, id: 14 },
            { x: 32.25, y: 61, id: 15 },
            ...generatePins(4, 42, 61, 9.5, 18)
        ], r: 4
    },
    "/images/Pico.svg": {
        width: 200, height: 80, pins: [
            ...generatePins(16, 186, 4.5, -9.6, 0, [2, 6, 10, 13]),
            ...generatePins(7, 4, 80 - 4.5 - 3, 9.6, 16, [18, 22]),
            ...generatePins(3, 4 + (10 * 9.6), 80 - 4.5 - 3, 9.6, 26, [28])
        ], r: 3
    },
    "/images/ArduinoMicro.svg": {
        width: 143, height: 51, pins: [
            ...generatePins(2, 130.10, 1.64, -7.18, 16),
            ...generatePins(2, 115.4, 1.64, -7.18, 0),
            ...generatePins(11, 86.6, 1.64, -7.18, 2),
            ...generatePins(2, 130.10, 45.64, -7.18, 15),
            ...generatePins(6, 36.6, 45.64, 7.18, 18),
            { x: 15, y: 45.64, id: 13 },
        ], r: 3.7
    },
    "/images/ArduinoLeonardo.svg": { width: 199, height: 150, pins: [...generatePins(8, 183, 5, -7, 0), ...generatePins(6, 121, 5, -7, 8), ...generatePins(6, 146, 141, 7, 18)], r: 4 },
}
var row = 15;
var rowADK = 13;
for (var i = 22; i < 53; i+=2) {
    pinLocations["/images/ArduinoMega.svg"].pins.push(...generatePins(2, 378.5, row, 9.5, i))
    pinLocations["/images/ArduinoMegaADK.svg"].pins.push(...generatePins(2, 283, rowADK, 7.2, i))
    row += 9.5;
    rowADK += 7.2;
}
var picoBindings = Object.assign({}, [...Array(28).keys()].map(s => "GP" + s))
picoBindings[255] = "Disabled"
var megaBindings = { 255: "Disabled" }
var megaBindingsAnalog = []
for (var i = 54; i < 70; i++) {
    megaBindingsAnalog.push(i);
    megaBindings[i] = "A" + (i - 54);
}
var bindings = {
    "/images/Pico.svg": picoBindings,
    "/images/ArduinoUno.svg": { 14: "A0", 15: "A1", 16: "A2", 17: "A3", 18: "A4", 19: "A5", 255: "Disabled" },
    "/images/ArduinoProMicro.svg": { 0: "TXO", 1: "RXI", 18: "A0", 19: "A1", 20: "A2", 21: "A3", 255: "Disabled" },
    "/images/ArduinoLeonardo.svg": { 18: "A0", 19: "A1", 20: "A2", 21: "A3", 22: "A4", 23: "A5", 255: "Disabled" },
    "/images/ArduinoMicro.svg": { 0: "TX1", 1: "RX1", 18: "A0", 19: "A1", 20: "A2", 21: "A3", 22: "A4", 23: "A5", 17: "SS", 16: "MOSI", 14: "MISO", 15: "SCK", 255: "Disabled" },
    "/images/ArduinoMega.svg": megaBindings,
    "/images/ArduinoMegaADK.svg": megaBindings,
}
var analog = {
    "/images/ArduinoLeonardo.svg": [18, 19, 20, 21, 22, 23, 4, 6, 7, 8, 9, 10, 12],
    "/images/ArduinoProMicro.svg": [18, 19, 20, 21, 22, 23, 4, 6, 7, 8, 9, 10, 12],
    "/images/ArduinoMicro.svg": [18, 19, 20, 21, 22, 23, 4, 6, 7, 8, 9, 10, 12],
    "/images/ArudinoUno.svg": [14, 15, 16, 17, 18, 19],
    "/images/Pico.svg": [26, 27, 28, 29],
    "/images/ArduinoMega.svg": megaBindingsAnalog,
    "/images/ArduinoMegaADK.svg": megaBindingsAnalog,
}
function checkValid(pin, board) {
    return analog[board].includes(pin);
}
function getBindings(board) {
    return (pin) => (bindings[board][pin] || pin);
}

var drumLabels = {
    "Up": "D-pad Up",
    "Down": "D-pad Down",
    "Left": "D-pad Left",
    "Right": "D-pad Right",
    "Start": "Start Button",
    "Back": "Select Button",
    "Home": "Home Button",
    "A": "Green Drum",
    "B": "Red Drum",
    "Y": "Yellow Drum",
    "X": "Blue Drum",
    "LB": "Orange Drum",
    "RB": "Kick Pedal",
    "LX": "Joystick X Axis",
    "LY": "Joystick Y Axis",
    "SP": "Star Power Output"
}
var guitarLabels = {
    "Up": "Strum Up",
    "Down": "Strum Down",
    "Left": "D-pad Left",
    "Right": "D-pad Right",
    "Start": "Start Button",
    "Back": "Select Button",
    "Home": "Home Button",
    "A": "Green Fret",
    "B": "Red Fret",
    "Y": "Yellow Fret",
    "X": "Blue Fret",
    "LB": "Orange Fret",
    "LX": "Joystick X Axis",
    "LY": "Joystick Y Axis",
    "RX": "Whammy",
    "RY": "Tilt Axis",
    "SP": "Star Power Output"
}
var djLabels = {
    "Up": "D-pad Up",
    "Down": "D-pad Down",
    "Left": "D-pad Left",
    "Right": "D-pad Right",
    "Start": "Start Button",
    "Back": "Back Button",
    "Home": "Home Button",
    "A": "A Button",
    "B": "B Button",
    "X": "X Button",
    "Y": "Y Button",
    "LeftStick": "Euphoria Button",
    "RX": "Crossfader",
    "RY": "Effects knob",
    "SP": "Euphoria Output"
}
var guitarRBLabels = {
    "Up": "Strum Up",
    "Down": "Strum Down",
    "Left": "DPad Left",
    "Right": "DPad Right",
    "Start": "Start Button",
    "Back": "Select Button",
    "Home": "Home Button",
    "A": "Green Fret",
    "B": "Red Fret",
    "Y": "Yellow Fret",
    "X": "Blue Fret",
    "LB": "Orange Fret",
    "Lt": "Effects Switch",
    "LX": "Joystick X Axis",
    "LY": "Joystick Y Axis",
    "RX": "Whammy",
    "RY": "Tilt",
    "SP": "Star Power Output"
}

var guitarLiveLabels = {
    "Up": "Strum Up",
    "Down": "Strum Down",
    "Left": "D-pad Left",
    "Right": "D-pad Right",
    "Start": "Start Button",
    "Back": "Select Button",
    "Home": "Home Button",
    "A": "Black 1",
    "B": "Black 2",
    "Y": "Black 3",
    "X": "White 1",
    "LB": "White 2",
    "RB": "White 3",
    "LX": "Joystick X Axis",
    "LY": "Joystick Y Axis",
    "RX": "Whammy",
    "RY": "Tilt Axis",
    "SP": "Star Power Output"
}
var wiiLabels = {
    "Lt": "Left Shoulder Axis",
    "Rt": "Right Shoulder Axis",
    "LX": "Left Joystick X Axis",
    "LY": "Left Joystick Y Axis",
    "RX": "Right Joystick X Axis",
    "RY": "Right Joystick Y Axis"
}
var wiiGuitarLabels = {
    "LX": "Joystick X Axis",
    "LY": "Joystick Y Axis",
    "RX": "Whammy",
    "RY": "Tilt Axis",
    "SP": "Star Power Output"
}
var mouseLabels = {
    "A": "Left Click",
    "B": "Right Click",
    "LX": "Mouse Movement X",
    "LY": "Mouse Movement Y",
    "RX": "Vertical Scroll",
    "RY": "Horizontal Scroll"
}

var defLabels = {
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
    "LX": "Left Joystick X Axis",
    "LY": "Left Joystick Y Axis",
    "RX": "Right Joystick X Axis",
    "RY": "Right Joystick Y Axis",
}

function getUnused(isGuitar, isWii, isLiveGuitar, isRB, isDrum, isMouse, isDJ) {
    let unused = [];
    let labels = getLabels(isGuitar, isWii, isLiveGuitar, isRB, isDrum, isMouse, isDJ);
    if (labels === defLabels) return unused;
    for (let label in defLabels) {
        if (!(label in labels)) {
            unused.push(label);
        }
    }
    return unused;
}

function getLabels(isGuitar, isWii, isLiveGuitar, isRB, isDrum, isMouse, isDJ) {
    if (isMouse) {
        return mouseLabels;
    }
    if (isWii) {
        if (isGuitar) return wiiGuitarLabels;
        return wiiLabels;
    }
    if (isDJ) return djLabels;
    if (isDrum) return drumLabels;
    if (isRB) return guitarRBLabels;
    if (isLiveGuitar) return guitarLiveLabels;
    if (isGuitar) return guitarLabels;
    return defLabels;
}
