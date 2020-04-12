.pragma library
.import net.tangentmc 1.0 as TangentMC

function fillCombobox(name) {
    var model = [];
    var entries = TangentMC.ArdwiinoDefines[`get_${name}_entries`]();
    for (let key in entries) {
        model.push({key: key, value: entries[key]});
    }
    return model;
}

var boardImages = {}
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_GAMEPAD] = "images/xbox.png";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_WHEEL] = "images/xbox.png";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_ARCADE_STICK] = "images/arcadestick.png";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_FLIGHT_STICK] = "images/flightstick.png";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_DANCE_PAD] = "images/ddr.png";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_LIVE_GUITAR] = "images/guitar.png";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_GUITAR] = "images/guitar.png";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_GUITAR_BASS] = "images/guitar.png";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_DRUMS] = "images/rb-drums.png";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_ARCADE_PAD] = "images/arcadestick.png";
boardImages[TangentMC.ArdwiinoDefinesValues.KEYBOARD] = "images/keyboard.png";
boardImages[TangentMC.ArdwiinoDefinesValues.SWITCH_GAMEPAD] = "images/controller.png";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_GAMEPAD] = "images/controller.png";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_GUITAR_HERO_GUITAR] = "images/guitar.png";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_ROCK_BAND_GUITAR] = "images/guitar.png";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_GUITAR_HERO_DRUMS] = "images/rb-drums.png";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_ROCK_BAND_DRUMS] = "images/rb-drums.png";


function getBoardImage(type) {
    return boardImages[type+""];
}
