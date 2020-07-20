#include "ardwiinolookup.h"

#include <QCoreApplication>
#include <QDir>
#include <QRegularExpression>

#include "QDebug"
#include <hidapi.h>
#ifdef Q_OS_MACOS
#include <libusb.h>
#else
#include <libusb-1.0/libusb.h>
#endif
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
    uint8_t major = (releaseID >> 8)  & 0xff;
    uint8_t minor = (releaseID >> 4) & 0xf;
    uint8_t revision = (releaseID) & 0xf;
    return QVersionNumber(major,minor, revision) < currentVersion;
}
auto ArdwiinoLookup::isOldAPIArdwiino(const QSerialPortInfo &serialPortInfo) -> bool {
    return isArdwiino(serialPortInfo) && serialPortInfo.serialNumber() == "1.2";
}
//Ardwino PS3 Controllers use sony vids. No other sony controller should expose a serial port however, so we should be fine doing this.
auto ArdwiinoLookup::isArdwiino(const QSerialPortInfo &serialPortInfo) -> bool {
    return serialPortInfo.vendorIdentifier() == HARMONIX_VID || serialPortInfo.vendorIdentifier() == SONY_VID || serialPortInfo.vendorIdentifier() == SWITCH_VID || (serialPortInfo.vendorIdentifier() == ARDWIINO_VID && serialPortInfo.productIdentifier() == ARDWIINO_PID);
}
auto ArdwiinoLookup::isArdwiino(const UsbDevice_t& usbDeviceId) -> bool {
    return usbDeviceId.vid == HARMONIX_VID || usbDeviceId.vid == SONY_VID || usbDeviceId.vid == SWITCH_VID || (usbDeviceId.vid == ARDWIINO_VID && usbDeviceId.pid == ARDWIINO_PID);
}
ArdwiinoLookup *ArdwiinoLookup::instance = nullptr;
const board_t ArdwiinoLookup::empty = {"", "", "", 0, {}, "", "", 0, "", false};
const board_t ArdwiinoLookup::boards[17] = {
    {"uno-atmega16u2", "uno-usb", "Arduino Uno", 57600, {0x2FEF}, "dfu", "atmega16u2", 16000000, "/images/ArduinoUno.svg", true, false},
    {"uno-at90usb82", "uno-usb", "Arduino Uno", 57600, {0x2FF7}, "dfu", "at90usb82", 16000000, "/images/ArduinoUno.svg", true, false},
    {"uno", "uno-main", "Arduino Uno", 115200, {0x0043, 0x7523, 0x0001, 0xea60, 0x0243}, "arduino", "atmega328p", 16000000, "/images/ArduinoUno.svg", true, false},
    {"a-micro", "a-micro", "Arduino Micro in DFU Mode", 57600, {0x0037}, "avr109", "atmega32u4", 8000000, "/images/ArduinoProMicro.svg", false, true},
    {"a-micro", "a-micro", "Arduino Micro", 57600, {0x8037}, "avr109", "atmega32u4", 8000000, "/images/ArduinoProMicro.svg", false, false},
    {"micro", "micro", "Arduino Pro Micro", 57600, {0x9204}, "avr109", "atmega32u4", 8000000, "/images/ArduinoProMicro.svg", false, false},
    {"micro", "micro", "Arduino Pro Micro", 57600, {0x9206}, "avr109", "atmega32u4", 16000000, "/images/ArduinoProMicro.svg", false, false},
    {"leonardo", "leonardo", "Arduino Leonardo", 57600, {0x8036, 0x800c}, "avr109", "atmega32u4", 16000000, "/images/ArduinoLeonardo.svg", false, false},
    {"leonardo", "leonardo", "Arduino Pro Micro / Leonardo in DFU Mode", 57600, {0x0036}, "avr109", "atmega32u4", 16000000, "/images/ArduinoLeonardo.svg", false, true},
    {"micro", "micro", "Arduino Pro Micro in DFU Mode", 57600, {0x9205}, "avr109", "atmega32u4", 8000000, "/images/ArduinoLeonardo.svg", false, true},
    {"micro", "micro", "Arduino Pro Micro in DFU Mode", 57600, {0x9203}, "avr109", "atmega32u4", 16000000, "/images/ArduinoLeonardo.svg", false, true},
    {"mega2560-atmega16u2", "mega2560-usb", "Arduino Mega 2560", 57600, {0x2FEF}, "dfu", "atmega16u2", 16000000, "/images/ArduinoMega.svg", true, false},
    {"mega2560-at90usb82", "mega2560-usb", "Arduino Mega 2560", 57600, {0x2FF7}, "dfu", "at90usb82", 16000000, "/images/ArduinoMega.svg", true, false},
    {"mega2560", "mega2560-main", "Arduino Mega 2560", 115200, {0x0010, 0x0042}, "arduino", "atmega328p", 16000000, "/images/ArduinoMega.svg", true, false},
    {"megaadk-atmega16u2", "megaadk-usb", "Arduino Mega ADK", 57600, {0x2FEF}, "dfu", "atmega16u2", 16000000, "/images/ArduinoMegaADK.svg", true, false},
    {"megaadk-at90usb82", "megaadk-usb", "Arduino Mega ADK", 57600, {0x2FF7}, "dfu", "at90usb82", 16000000, "/images/ArduinoMegaADK.svg", true, false},
    {"megaadk", "megaadk-main", "Arduino Mega ADK", 115200, {0x003f, 0x0044}, "arduino", "atmega328p", 16000000, "/images/ArduinoMegaADK.svg", true, false},
};

auto ArdwiinoLookup::findByBoard(const QString &board_name, bool inBootloader) -> const board_t {
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
    return empty;
}
auto ArdwiinoLookup::detectBoard(struct hid_device_info *usbDeviceId) -> const board_t {
    for (const auto &board : boards) {
        for (auto &pid : board.productIDs) {
            if (pid && pid == usbDeviceId->product_id) {
                return board;
            }
        }
    }
    return empty;
}
auto ArdwiinoLookup::getInstance() -> ArdwiinoLookup * {
    if (!ArdwiinoLookup::instance) {
        ArdwiinoLookup::instance = new ArdwiinoLookup();
    }
    return ArdwiinoLookup::instance;
}
