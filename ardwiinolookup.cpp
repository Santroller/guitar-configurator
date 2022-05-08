#include "ardwiinolookup.h"

#include <QCoreApplication>
#include <QDir>
#include <QRegularExpression>

#include "QDebug"
QVersionNumber ArdwiinoLookup::currentVersion = QVersionNumber(-1);
const static auto versionRegex = QRegularExpression("^version-([\\d.]+)$");
ArdwiinoLookup::ArdwiinoLookup(QObject *parent) : QObject(parent) {
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cd("firmware");
    QFile f(dir.filePath("version"));
    f.open(QFile::ReadOnly | QFile::Text);
    auto match2 = versionRegex.match(f.readAll());
    currentVersion = QVersionNumber::fromString(match2.captured(1));
}
auto ArdwiinoLookup::isOutdatedArdwiino(const unsigned short releaseID) -> bool {
    uint8_t major = (releaseID >> 8) & 0xff;
    uint8_t minor = (releaseID >> 4) & 0xf;
    uint8_t revision = (releaseID)&0xf;
    return QVersionNumber(major, minor, revision) < currentVersion;
}
auto ArdwiinoLookup::isOldAPIArdwiino(const QSerialPortInfo &serialPortInfo) -> bool {
    return isArdwiino(serialPortInfo) && serialPortInfo.serialNumber() == "1.2";
}
// Ardwino PS3 Controllers use sony vids. No other sony controller should expose a serial port however, so we should be fine doing this.
auto ArdwiinoLookup::isArdwiino(const QSerialPortInfo &serialPortInfo) -> bool {
    return serialPortInfo.vendorIdentifier() == HARMONIX_VID || serialPortInfo.vendorIdentifier() == SONY_VID || serialPortInfo.vendorIdentifier() == SWITCH_VID || (serialPortInfo.vendorIdentifier() == ARDWIINO_VID && serialPortInfo.productIdentifier() == ARDWIINO_PID);
}
auto ArdwiinoLookup::isArdwiino(const UsbDevice_t &usbDeviceId) -> bool {
    return usbDeviceId.vid == HARMONIX_VID || usbDeviceId.vid == SONY_VID || usbDeviceId.vid == SWITCH_VID || (usbDeviceId.vid == ARDWIINO_VID && usbDeviceId.pid == ARDWIINO_PID);
}
ArdwiinoLookup *ArdwiinoLookup::instance = nullptr;
const board_t ArdwiinoLookup::empty = {"", "", "", 0, {}, "", "", 0, "", false, false};
const board_t ArdwiinoLookup::boards[69] = {
    {"uno-atmega16u2", "uno-usb", "Arduino Uno", 57600, {0x2FEF}, "dfu", "atmega16u2", 16000000, "/images/ArduinoUno.svg", true, false},
    {"uno-at90usb82", "uno-usb", "Arduino Uno", 57600, {0x2FF7}, "dfu", "at90usb82", 16000000, "/images/ArduinoUno.svg", true, false},
    {"uno", "uno-main", "Arduino Uno", 115200, {0x0043, 0x7523, 0x0001, 0xea60, 0x0243}, "arduino", "atmega328p", 16000000, "/images/ArduinoUno.svg", true, false},
    {"mini", "mini-main", "Arduino Pro Mini", 57600, {}, "arduino", "atmega328p", 16000000, "/images/ArduinoUno.svg", false, true},
    {"a-micro", "a-micro", "Arduino Micro in Bootloader Mode", 57600, {0x0037, 0x0237}, "avr109", "atmega32u4", 16000000, "/images/ArduinoMicro.svg", false, true},
    {"a-micro", "a-micro", "Arduino Micro", 57600, {0x8037, 0x8237}, "avr109", "atmega32u4", 16000000, "/images/ArduinoMicro.svg", false, false},
    {"micro", "micro", "Arduino Pro Micro", 57600, {0x9204}, "avr109", "atmega32u4", 8000000, "/images/ArduinoProMicro.svg", false, false},
    {"micro", "micro", "Arduino Pro Micro", 57600, {0x9206}, "avr109", "atmega32u4", 16000000, "/images/ArduinoProMicro.svg", false, false},
    {"leonardo", "leonardo", "Arduino Leonardo", 57600, {0x8036, 0x800c}, "avr109", "atmega32u4", 16000000, "/images/ArduinoLeonardo.svg", false, false},
    {"leonardo", "leonardo", "Arduino Micro / Pro Micro / Leonardo in Bootloader Mode", 57600, {0x0036}, "avr109", "atmega32u4", 16000000, "/images/ArduinoLeonardo.svg", false, true},
    {"micro", "micro", "Arduino Pro Micro in Bootloader Mode", 57600, {0x9203, 0x9207}, "avr109", "atmega32u4", 8000000, "/images/ArduinoLeonardo.svg", false, true},
    {"micro", "micro", "Arduino Pro Micro in Bootloader Mode", 57600, {0x9205}, "avr109", "atmega32u4", 16000000, "/images/ArduinoLeonardo.svg", false, true},
    {"mega2560-atmega16u2", "mega2560-usb", "Arduino Mega 2560", 57600, {0x2FEF}, "dfu", "atmega16u2", 16000000, "/images/ArduinoMega.svg", true, false},
    {"mega2560-at90usb82", "mega2560-usb", "Arduino Mega 2560", 57600, {0x2FF7}, "dfu", "at90usb82", 16000000, "/images/ArduinoMega.svg", true, false},
    {"mega2560", "mega2560-main", "Arduino Mega 2560", 115200, {0x0010, 0x0042}, "wiring", "atmega2560", 16000000, "/images/ArduinoMega.svg", true, false},
    {"megaadk-atmega16u2", "megaadk-usb", "Arduino Mega ADK", 57600, {0x2FEF}, "dfu", "atmega16u2", 16000000, "/images/ArduinoMegaADK.svg", true, false},
    {"megaadk-at90usb82", "megaadk-usb", "Arduino Mega ADK", 57600, {0x2FF7}, "dfu", "at90usb82", 16000000, "/images/ArduinoMegaADK.svg", true, false},
    {"megaadk", "megaadk-main", "Arduino Mega ADK", 115200, {0x003f, 0x0044}, "wiring", "atmega2560", 16000000, "/images/ArduinoMegaADK.svg", true, false},
    {"pico", "pico", "Raspberry PI Pico", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"adafruit_feather_rp2040", "adafruit_feather_rp2040", "Adafruit Feather RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"adafruit_itsybitsy_rp2040", "adafruit_itsybitsy_rp2040", "Adafruit ItsyBitsy RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"adafruit_feather_rp2040", "adafruit_feather_rp2040", "Adafruit KB2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"adafruit_qtpy_rp2040", "adafruit_qtpy_rp2040", "Adafruit QT Py RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"adafruit_trinkey_qt2040", "adafruit_trinkey_qt2040", "Adafruit Trinkey QT2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"arduino_nano_rp2040_connect", "arduino_nano_rp2040_connect", "Arduino Nano RP2040 Connect", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"melopero_shake_rp2040", "melopero_shake_rp2040", "Melopero Shake RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"pimoroni_interstate75_rp2040", "pimoroni_interstate75", "Pimoroni Interstate 75", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"pimoroni_keybow2040", "pimoroni_keybow2040", "Pimoroni Keybow 2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"pimoroni_pga2040", "pimoroni_pga2040", "Pimoroni PGA2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"pimoroni_picolipo_4mb", "pimoroni_picolipo_4mb", "Pimoroni Pico LiPo (4MB)", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"pimoroni_picolipo_16mb", "pimoroni_picolipo_16mb", "Pimoroni Pico LiPo (16MB)", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"pimoroni_picosystem_rp2040", "pimoroni_picosystem", "Pimoroni PicoSystem", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"pimoroni_plasma2040", "pimoroni_plasma2040", "Pimoroni Plasma 2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"pimoroni_tiny2040", "pimoroni_tiny2040", "Pimoroni Tiny 2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"pybstick26_rp2040", "pybstick26_rp2040", "RP2040 PYBStick", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"sparkfun_micromod_rp2040", "sparkfun_micromod", "SparkFun MicroMod - RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"sparkfun_promicro_rp2040", "sparkfun_promicro", "SparkFun Pro Micro - RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"sparkfun_thingplus_rp2040", "sparkfun_thingplus", "SparkFun Thing Plus - RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"vgaboard_rp2040", "vgaboard", "Pimoroni Pico VGA Demo Base", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"waveshare_rp2040_lcd_0.96", "waveshare_rp2040_lcd_0.96", "Waveshare RP2040-LCD-0.96", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"waveshare_rp2040_plus_4mb", "waveshare_rp2040_plus_4mb", "Waveshare RP2040-Plus (4MB)", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"waveshare_rp2040_plus_16mb", "waveshare_rp2040_plus_16mb", "Waveshare RP2040-Plus (16MB)", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"waveshare_rp2040_zero", "waveshare_rp2040_zero", "Waveshare RP2040-Zero", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"adafruit_feather_rp2040_bootloader", "adafruit_feather_rp2040", "Adafruit Feather RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"adafruit_itsybitsy_rp2040_bootloader", "adafruit_itsybitsy_rp2040", "Adafruit ItsyBitsy RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"adafruit_feather_rp2040_bootloader", "adafruit_feather_rp2040", "Adafruit KB2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"adafruit_qtpy_rp2040_bootloader", "adafruit_qtpy_rp2040", "Adafruit QT Py RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"adafruit_trinkey_qt2040_bootloader", "adafruit_trinkey_qt2040", "Adafruit Trinkey QT2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"arduino_nano_rp2040_connect_bootloader", "arduino_nano_rp2040_connect", "Arduino Nano RP2040 Connect", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"melopero_shake_rp2040_bootloader", "melopero_shake_rp2040", "Melopero Shake RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"pimoroni_interstate75_rp2040_bootloader", "pimoroni_interstate75", "Pimoroni Interstate 75", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"pimoroni_keybow2040_bootloader", "pimoroni_keybow2040", "Pimoroni Keybow 2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"pimoroni_pga2040_bootloader", "pimoroni_pga2040", "Pimoroni PGA2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"pimoroni_picolipo_4mb_bootloader", "pimoroni_picolipo_4mb", "Pimoroni Pico LiPo (4MB)", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"pimoroni_picolipo_16mb_bootloader", "pimoroni_picolipo_16mb", "Pimoroni Pico LiPo (16MB)", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"pimoroni_picosystem_rp2040_bootloader", "pimoroni_picosystem", "Pimoroni PicoSystem", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"pimoroni_plasma2040_bootloader", "pimoroni_plasma2040", "Pimoroni Plasma 2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"pimoroni_tiny2040_bootloader", "pimoroni_tiny2040", "Pimoroni Tiny 2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"pybstick26_rp2040_bootloader", "pybstick26_rp2040", "RP2040 PYBStick", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"sparkfun_micromod_rp2040_bootloader", "sparkfun_micromod", "SparkFun MicroMod - RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"sparkfun_promicro_rp2040_bootloader", "sparkfun_promicro", "SparkFun Pro Micro - RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"sparkfun_thingplus_rp2040_bootloader", "sparkfun_thingplus", "SparkFun Thing Plus - RP2040", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"vgaboard_rp2040_bootloader", "vgaboard", "Pimoroni Pico VGA Demo Base", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"waveshare_rp2040_lcd_0.96_bootloader", "waveshare_rp2040_lcd_0.96", "Waveshare RP2040-LCD-0.96", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"waveshare_rp2040_plus_4mb_bootloader", "waveshare_rp2040_plus_4mb", "Waveshare RP2040-Plus (4MB)", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"waveshare_rp2040_plus_16mb_bootloader", "waveshare_rp2040_plus_16mb", "Waveshare RP2040-Plus (16MB)", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"waveshare_rp2040_zero_bootloader", "waveshare_rp2040_zero", "Waveshare RP2040-Zero", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"pico-bootloader", "pico", "Raspberry PI Pico", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"generic", "generic", "Generic Serial Device", 0, {}, "arduino", "", 0, "/images/ArduinoUno.svg", false, false},
};
auto ArdwiinoLookup::findByBoard(const QString &board_name, bool inBootloader) -> const board_t {
    // the mini and is always in bootloader mode
    if (board_name == "mini") inBootloader = true;
    for (const auto &board : boards) {
        if (board.shortName == board_name && board.inBootloader == inBootloader) {
            return board;
        }
    }
    return empty;
}

auto ArdwiinoLookup::detectBoard(const QSerialPortInfo &serialPortInfo) -> const board_t {
    for (const auto &board : boards) {
        for (auto &pid : board.productIDs) {
            if (pid && pid == serialPortInfo.productIdentifier()) {
                return board;
            }
        }
    }
    return board_t(boards[(sizeof(boards) / sizeof(board_t)) - 1]);
}
auto ArdwiinoLookup::getInstance() -> ArdwiinoLookup * {
    if (!ArdwiinoLookup::instance) {
        ArdwiinoLookup::instance = new ArdwiinoLookup();
    }
    return ArdwiinoLookup::instance;
}
