.pragma library
.import net.tangentmc 1.0 as TangentMC

function fillCombobox(name) {
    var model = [];
    var entries = TangentMC.ArdwiinoDefines[`get_${name}_entries`]();
    for (let key in entries) {
        model.push({key: key, value: entries[key]});
    }
    model.sort((s,s2)=>s.value-s2.value);
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
//boardImages[TangentMC.ArdwiinoDefinesValues.KEYBOARD] = "images/Keyboard";
boardImages[TangentMC.ArdwiinoDefinesValues.SWITCH_GAMEPAD] = "images/SwitchCtrl";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_GAMEPAD] = "images/PS3Ctrl";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_GUITAR_HERO_GUITAR] = "images/GuitarHero/PS3/LesPaul";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_ROCK_BAND_GUITAR] = "images/RockBand/PS3/Guitar";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_GUITAR_HERO_DRUMS] = "images/GuitarHero/PS3/Drum";
boardImages[TangentMC.ArdwiinoDefinesValues.PS3_ROCK_BAND_DRUMS] = "images/RockBand/PS3/Drum";
boardImages[TangentMC.ArdwiinoDefinesValues.WII_ROCK_BAND_GUITAR] = "images/RockBand/Wii/Guitar";
boardImages[TangentMC.ArdwiinoDefinesValues.WII_ROCK_BAND_DRUMS] = "images/RockBand/Wii/Drum";
boardImages[TangentMC.ArdwiinoDefinesValues.MIDI_GUITAR_HERO_GUITAR] = "images/GuitarHero/360/LesPaul";
boardImages[TangentMC.ArdwiinoDefinesValues.MIDI_GUITAR_HERO_DRUMS] = "images/GuitarHero/360/Drum";
boardImages[TangentMC.ArdwiinoDefinesValues.MIDI_ROCK_BAND_GUITAR] = "images/RockBand/360/LesPaul";
boardImages[TangentMC.ArdwiinoDefinesValues.MIDI_ROCK_BAND_DRUMS] = "images/RockBand/360/Drum";
boardImages[TangentMC.ArdwiinoDefinesValues.MIDI_CONTROLLER] = "images/360Ctrl";
function findTypeDevice(id) {
    var st = getTypeCombobox();
    for (var type in st) {
        var t = st[type];
        var key = t.find(v => v.value === id);
        if (key) {
            return [type,key.key];
        }
    }
}

function getTypeCombobox() {
    var st = fillCombobox("SubType");
    var keys = [...new Set(Object.values(st).map(k=>k.key.split(" ")[0]))];
    var values = st.reduce((obj,value)=>{
                               var key = value.key.split(" ")[0];
                               value.key = value.key.split(" ").splice(1).join(" ");
                               var arr = [];
                               if (obj.hasOwnProperty(key)) {
                                   arr = obj[key];
                               }
                               arr.push(value);
                               obj[key]=arr;
                               return obj;
                           },{});
    return values;
}

console.log(findTypeDevice(9));
function getBoardImage(type) {
    return getBoardBase(type)+"/Base.svg";
}

function getBoardComponents(type) {
    return getBoardBase(type)+"/components/";
}

function getBoardBase(type) {
    let a = findTypeDevice(type);
    a[1] = a[1].split(" ").join("");
    return "images/"+a.join("/");
//    return boardImages[type+""];
}
