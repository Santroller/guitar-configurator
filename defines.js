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
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_GAMEPAD] = "images/360Ctrl";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_WHEEL] = "images/360Accessories/Wheel";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_ARCADE_STICK] = "images/360Accessories/ArcadeStick";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_FLIGHT_STICK] = "images/360Accessories/FlightStick";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_DANCE_PAD] = "images/360Accessories/DancePad";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_LIVE_GUITAR] = "images/GuitarHero/Live";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_GUITAR_HERO_GUITAR] = "images/GuitarHero/360/LesPaul";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_ROCK_BAND_GUITAR] = "images/RockBand/360/Guitar";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_ROCK_BAND_DRUMS] = "images/RockBand/360/Drum";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_GUITAR_HERO_DRUMS] = "images/GuitarHero/360/Drum";
boardImages[TangentMC.ArdwiinoDefinesValues.XINPUT_ARCADE_PAD] = "images/360Accessories/ArcadePad";
boardImages[TangentMC.ArdwiinoDefinesValues.KEYBOARD] = "images/Keyboard";
boardImages[TangentMC.ArdwiinoDefinesValues.SWITCH_GAMEPAD] = "images/SwitchCtrl";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_GAMEPAD] = "images/PS3Ctrl";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_GUITAR_HERO_GUITAR] = "images/GuitarHero/PS3/LesPaul";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_ROCK_BAND_GUITAR] = "images/RockBand/PS3/Guitar";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_GUITAR_HERO_DRUMS] = "images/GuitarHero/PS3/Drum";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_ROCK_BAND_DRUMS] = "images/RockBand/PS3/Drum";
boardImages[TangentMC.ArdwiinoDefinesValues.WII_ROCK_BAND_GUITAR] = "images/RockBand/Wii/Guitar";
boardImages[TangentMC.ArdwiinoDefinesValues.WII_ROCK_BAND_DRUMS] = "images/RockBand/Wii/Drum";


function getBoardImage(type) {
    return boardImages[type+""]+"/Base.svg";
}
