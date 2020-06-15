#include "ardwiinolookup.h"

#include <QCoreApplication>
#include <QDir>
#include <QRegularExpression>

#include "QDebug"
#include <hidapi.h>
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

auto ArdwiinoLookup::isOldAPIArdwiino(const QSerialPortInfo &serialPortInfo) -> bool {
    return isArdwiino(serialPortInfo) && serialPortInfo.serialNumber() == "1.2";
}
//Ardwino PS3 Controllers use sony vids. No other sony controller should expose a serial port however, so we should be fine doing this.
auto ArdwiinoLookup::isArdwiino(const QSerialPortInfo &serialPortInfo) -> bool {
    return serialPortInfo.vendorIdentifier() == HARMONIX_VID || serialPortInfo.vendorIdentifier() == SONY_VID || serialPortInfo.vendorIdentifier() == SWITCH_VID || (serialPortInfo.vendorIdentifier() == ARDWIINO_VID && serialPortInfo.productIdentifier() == ARDWIINO_PID);
}
auto ArdwiinoLookup::isArdwiino(struct hid_device_info *usbDeviceId) -> bool {
    return usbDeviceId->vendor_id == HARMONIX_VID || usbDeviceId->vendor_id == SONY_VID || usbDeviceId->vendor_id == SWITCH_VID || (usbDeviceId->vendor_id == ARDWIINO_VID && usbDeviceId->product_id == ARDWIINO_PID);
}
auto ArdwiinoLookup::isAreadyDFU(const QSerialPortInfo &serialPortInfo) -> bool {
    return serialPortInfo.productIdentifier() == 0x0036 || serialPortInfo.productIdentifier() == 0x9207;
}
ArdwiinoLookup *ArdwiinoLookup::instance = nullptr;
const board_t ArdwiinoLookup::empty = {"", "", "", 0, {}, "", "", 0, "", false};
const board_t ArdwiinoLookup::boards[11] = {
    {"uno-atmega16u2", "uno-usb", "Arduino Uno", 57600, {0x2FEF}, "dfu", "atmega16u2", 16000000, "/images/ArduinoUno.svg", true},
    {"uno-at90usb82", "uno-usb", "Arduino Uno", 57600, {0x2FF7}, "dfu", "at90usb82", 16000000, "/images/ArduinoUno.svg", true},
    {"uno", "uno-main", "Arduino Uno", 115200, {0x0043, 0x7523, 0x0001, 0xea60, 0x0243}, "arduino", "atmega328p", 16000000, "/images/ArduinoUno.svg", true},
    {"micro", "micro", "Arduino Pro Micro", 57600, {0x9203, 0x9204, 0x9205, 0x9206, 0x9207, 0x9208}, "avr109", "atmega32u4", 8000000, "/images/ArduinoProMicro.svg", false},
    {"leonardo", "leonardo", "Arduino Leonardo", 57600, {0x0036, 0x8036, 0x800c}, "avr109", "atmega32u4", 16000000, "/images/ArduinoLeonardo.svg", false},
    {"mega2560-atmega16u2", "mega2560-usb", "Arduino Mega 2560", 57600, {0x2FEF}, "dfu", "atmega16u2", 16000000, "/images/ArduinoMega.svg", true},
    {"mega2560-at90usb82", "mega2560-usb", "Arduino Mega 2560", 57600, {0x2FF7}, "dfu", "at90usb82", 16000000, "/images/ArduinoMega.svg", true},
    {"mega2560", "mega2560-main", "Arduino Mega 2560", 115200, {0x0010, 0x0042}, "arduino", "atmega328p", 16000000, "/images/ArduinoMega.svg", true},
    {"megaadk-atmega16u2", "megaadk-usb", "Arduino Mega ADK", 57600, {0x2FEF}, "dfu", "atmega16u2", 16000000, "/images/ArduinoMegaADK.svg", true},
    {"megaadk-at90usb82", "megaadk-usb", "Arduino Mega ADK", 57600, {0x2FF7}, "dfu", "at90usb82", 16000000, "/images/ArduinoMegaADK.svg", true},
    {"megaadk", "megaadk-main", "Arduino Mega ADK", 115200, {0x003f, 0x0044}, "arduino", "atmega328p", 16000000, "/images/ArduinoMegaADK.svg", true},
};

auto ArdwiinoLookup::findByBoard(const QString &board_name) -> const board_t {
    for (const auto &board : boards) {
        if (board.shortName == board_name) {
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
