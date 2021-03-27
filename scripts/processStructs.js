// Turn config.h into a Q_OBJECT that wraps Configuration_t with getters and setters.
// Doing things this way allows us to easily handle new config data when it is added.
const fs = require("fs");
let config = fs.readFileSync("../submodules/Ardwiino/src/shared/config/config.h").toString();
let matches = Array.from(config.matchAll(/\s\s(\S+) ([^;\n]+);/gi));
let types = {};
for (let match of matches) {
    let struct = config.substring(match.index).split("}")[1].split(/\s+/)[1].replace(";", "");
    if (!struct.includes("int") && struct.endsWith("_t")) {
        if (!types[struct]) {
            types[struct] = [];
        }
        types[struct].push({ type: match[1], name: match[2] })
    }
}
function capFirst(str) {
    if (str.includes("_")) {
        return str.split("_").map(capFirst).join("");
    }
    return str.charAt(0).toUpperCase() + str.substring(1);
}
function lowFirst(str) {
    return str.charAt(0).toLowerCase() + str.substring(1);
}
let enumTypes = {
    "m_config.main.inputType": "ArdwiinoDefines::InputType",
    "m_config.main.subType": "ArdwiinoDefines::SubType",
    "m_config.main.tiltType": "ArdwiinoDefines::TiltType",
    "m_config.main.fretLEDMode": "ArdwiinoDefines::FretLedMode",
    "m_config.main.signature": "ArdwiinoDefines::FirmwareType",
    "m_config.axis.mpu6050Orientation": "ArdwiinoDefines::GyroOrientation"
};
function recurseMatches(type, path, func, result) {
    if (type.includes("int") || !type.endsWith("_t") || type == "Led_t") {
        let isArray = false;
        let cast = "";
        let cast2 = "";
        if (path.includes("[")) {
            path = path.split("[")[0];
            func = func.split("[")[0];
            isArray = true;
        }
        let e = enumTypes[path];
        if (e) {
            cast = `(${e})`;
            type = e;
        }
        if (type.includes("int")) {
            cast = `(${type})`;
            type = "int";
        }
        if (type == "Led_t") {
            cast = `*(Led_t*)&`;
            cast2 = `*(uint32_t*)&`;
            type = "int";
        }
        if (func.endsWith("Pin")) {
            func = func.substring(0,func.length-3);
        }
        result.push({ path, type, func, cast, cast2, isArray });
        return;
    }
    for (let subtype of types[type]) {
        recurseMatches(subtype.type, path + "." + subtype.name, func + capFirst(subtype.name), result);
    }

}
let paths = [];
let pins = [];
for (let match of matches) {
    let struct = config.substring(match.index).split("}")[1].split(/\s+/)[1].replace(";", "");
    if (struct == "Pins_t") {
        pins.push(`"${capFirst(match[2])}"`);
    }
    if (struct != "Configuration_t") {
        continue;
    }
    let type = match[1];
    recurseMatches(type, "m_config." + match[2], capFirst(match[2]), paths);
    // out += (path, match[2]);
}
let out = (`
#include <QDebug>
#include <QMetaEnum>
#include <QObject>
#include "ardwiino_defines.h"

#include "submodules/Ardwiino/src/shared/config/config.h"
class DeviceConfiguration : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool hasChanged MEMBER m_hasChanged NOTIFY hasChangedUpdated)
    Q_PROPERTY(bool isGuitar READ isGuitar NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isDrum READ isDrum NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isLiveGuitar READ isLiveGuitar NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isKeyboard READ isKeyboard NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isXInput READ isXInput NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isRB READ isRB NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isWii READ isWii NOTIFY mainInputTypeUpdated)
    Q_PROPERTY(bool isMIDI READ isMIDI NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isMouse READ isMouse NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool hasAddressableLEDs READ hasAddressableLEDs NOTIFY mainFretLEDModeUpdated)
    Q_PROPERTY(QVariantList leds READ getLEDs NOTIFY ledsUpdated)
    Q_PROPERTY(QVariantMap ledColours READ getLEDColours NOTIFY ledsUpdated)
`)
for (let path of paths) {
    if (path.isArray) {
        // Leds are not stored in pin order, so this type of mapping does not make sense, as a result leds is defined above and has its own getter
        if (path.func == "Leds") continue;
        out += (`    Q_PROPERTY(QVariantMap ${lowFirst(path.func)} READ get${path.func} NOTIFY ${lowFirst(path.func)}Updated)
`);
    } else {
        out += (`    Q_PROPERTY(${path.type} ${lowFirst(path.func)} READ get${path.func} WRITE set${path.func} NOTIFY ${lowFirst(path.func)}Updated)
`);
    }
}
out += (`
public:
    explicit DeviceConfiguration(Configuration_t config, QObject* parent = nullptr);
public slots:
    bool isGuitar() {
        return ArdwiinoDefines::getName(getMainSubType()).toLower().contains("guitar");
    }
    bool isDrum() {
        return ArdwiinoDefines::getName(getMainSubType()).toLower().contains("drum");
    }
    bool isMIDI() {
        return ArdwiinoDefines::getName(getMainSubType()).toLower().contains("midi");
    }
    bool isKeyboard() {
        return getMainSubType() >= ArdwiinoDefines::KEYBOARD_GAMEPAD && getMainSubType() <= ArdwiinoDefines::KEYBOARD_ROCK_BAND_DRUMS;
    }
    bool isMouse() {
        return getMainSubType() == ArdwiinoDefines::MOUSE;
    }
    bool isLiveGuitar() {
        return getMainSubType() == ArdwiinoDefines::XINPUT_LIVE_GUITAR;
    }
    bool isWii() {
        return getMainInputType() == ArdwiinoDefines::WII;
    }
    bool isRB() {
        return ArdwiinoDefines::getName(getMainSubType()).toLower().contains("rock");
    }
    bool isXInput() {
        return ArdwiinoDefines::getName(getMainSubType()).toLower().contains("xinput");
    }
    bool hasAddressableLEDs() {
        return getMainFretLEDMode() == ArdwiinoDefines::APA102;
    }
    Configuration_t getConfig() const {
        return m_config;
    }
`)
for (let path of paths) {
    if (path.isArray) {
        // Leds are not stored in pin order, so this type of mapping does not make sense, as a result LED methods are defined manually below.
        if (path.func == "Leds") continue;
        out += (`
    QVariantMap get${path.func}() const {
        QVariantMap l;
        for (auto pin: pins) {
            l[pin] = ${path.cast2}${path.path};
        }
        return l;
    }`);
        out += (`
    void set${path.func}ValueAt(int i, ${path.type} val) {
        if (${path.path}[i] != ${path.cast}val) {
            m_hasChanged = true;
            ${path.path}[i] = ${path.cast}val;
            emit ${lowFirst(path.func)}Updated();
            emit hasChangedUpdated();
        }
    }
    void set${path.func}Value(QString key, ${path.type} val) {
        if (${path.path}[pins.indexOf(key)] != ${path.cast}val) {
            ${path.path}[pins.indexOf(key)] = ${path.cast}val;
            m_hasChanged = true;
            emit ${lowFirst(path.func)}Updated();
            m_hasChanged = true;
            emit hasChangedUpdated();
        }
    }`);
     
    } else {
        out += (`
    ${path.type} get${path.func}() const {
        return ${path.cast}${path.path};
    }`);
        out += (`
    void set${path.func}(${path.type} val) {
        if (${path.path} != ${path.cast}val) {
            ${path.path} = ${path.cast}val;
            emit ${lowFirst(path.func)}Updated();
            m_hasChanged = true;
            emit hasChangedUpdated();
        }
    }`);
    }
}
out += (`
    void setLED(QString key, int color) {
        uint32_t ucolor = color;
        auto pin = pins.indexOf(key)+1;
        for (auto& led : m_config.leds) {
            // Either update the colour of the led if it is already in the map, or set the first unused led.
            if (led.pin == pin || led.pin == 0) {
                led.pin = pin;
                led.red = ucolor >> 16 & 0xff;
                led.green = ucolor >> 8 & 0xff;
                led.blue = ucolor & 0xff;
                emit ledsUpdated();
                m_hasChanged = true;
                emit hasChangedUpdated();
                return;
            }
        }
    }
    QVariantMap getLEDColours() {
        QVariantMap l;
        for (auto led : m_config.leds) {
            if (led.pin == 0) break;
            l[pins[led.pin-1]] = led.red << 16 | led.green << 8 | led.blue;
        }
        for (auto pin: pins) {
            l[pin] = l.value(pin,0);
        }
        return l;
    }
    QVariantList getLEDs() {
        QVariantList l;
        for (auto led : m_config.leds) {
            if (led.pin == 0) break;
            l << pins[led.pin-1];
        }
        return l;
    }
    void clearLED(QString key) {
        auto pin = pins.indexOf(key)+1;
        auto a = std::remove_if(std::begin(m_config.leds), std::end(m_config.leds), [pin](Led_t x) { return x.pin == pin; });
        Led_t empty = {0,0,0};
        std::fill(a, std::end(m_config.leds), empty);
        std::copy(std::begin(m_config.leds), std::end(m_config.leds), std::begin(m_config.leds));
        emit ledsUpdated();
        m_hasChanged = true;
        emit hasChangedUpdated();
    }
    void moveLED(int from, int to) {
        if (from == to)
            return;
        auto const b = std::begin(m_config.leds);
        if (from < to)
            std::rotate(b + from, b + from + 1, b + to + 1);
        else
            std::rotate(b + to, b + from, b + from + 1);
        emit ledsUpdated();
        m_hasChanged = true;
        emit hasChangedUpdated();
    }

    QMap<QString, uint> getMappings() {
        QMap<QString, uint> map;
        PinsCombined_t* combined = reinterpret_cast<PinsCombined_t*>(&m_config.pins);
        for (uint8_t i = 0; i < pins.size(); i++) {
            if (i < XBOX_BTN_COUNT) {
                map[pins[i]] = combined->buttons[i];
            } else {
                map[pins[i]] = combined->axis[i-XBOX_BTN_COUNT].pin;
            }
        }
        return map;
    }
signals:
    void hasChangedUpdated();
`)
for (let path of paths) {
    out += (`    void ${lowFirst(path.func)}Updated();
`);
}
// There is an extra SP pin that isnt part of Pins_t, so add it manually
pins.push(`"SP"`);
out += (`
private:
    Configuration_t m_config;
    const static QStringList pins;
    bool m_hasChanged;
};`)

fs.writeFileSync("../deviceconfiguration.h", out);

out = `#include "deviceconfiguration.h"
DeviceConfiguration::DeviceConfiguration(Configuration_t config, QObject* parent):QObject(parent),m_config(config) {
};
const QStringList DeviceConfiguration::pins = {${pins.toString()}};`
fs.writeFileSync("../deviceconfiguration.cpp", out);