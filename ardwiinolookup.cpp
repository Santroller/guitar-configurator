#include "ardwiinolookup.h"
#include "QDebug"
#include "wii_extensions.h"
#include "input_types.h"
#include <QRegularExpression>
#include <QCoreApplication>
#include <QDir>
QVersionNumber ArdwiinoLookup::currentVersion = QVersionNumber(-1);
const static auto versionRegex = QRegularExpression("^version-([\\d.]+)$");
ArdwiinoLookup::ArdwiinoLookup(QObject *parent):QObject(parent) {
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cd("firmware");
    QFile f(dir.filePath("version"));
    f.open(QFile::ReadOnly | QFile::Text);
    auto match2 = versionRegex.match(f.readAll());
    currentVersion = QVersionNumber::fromString(match2.captured(1));
}

auto ArdwiinoLookup::lookupExtension(uint8_t type, uint16_t device) -> QString {
    auto vtype = InputTypes::Value(type);
    if (vtype == InputTypes::WII_TYPE) {
        return QString("%1 %2").arg(InputTypes::toString(vtype)).arg(WiiExtensions::toString(WiiExtensions::Value(device)));
    }
    return InputTypes::toString(vtype);
}
auto ArdwiinoLookup::lookupType(uint8_t type) -> QString {
    return Controllers::toString(Controllers::Value(type));
}

auto ArdwiinoLookup::isOldArdwiino(const QSerialPortInfo& serialPortInfo) -> bool {
    auto match = versionRegex.match(serialPortInfo.serialNumber().toLower());
    return isArdwiino(serialPortInfo) && match.hasMatch() && QVersionNumber::fromString(match.captured(1)) < currentVersion;
}

auto ArdwiinoLookup::isOldFirmwareArdwiino(const QSerialPortInfo& serialPortInfo) -> bool {
    return isArdwiino(serialPortInfo) && !versionRegex.match(serialPortInfo.serialNumber().toLower()).hasMatch();
}
//Ardwino PS3 Controllers use sony vids. No other sony controller should expose a serial port however, so we should be fine doing this.
auto ArdwiinoLookup::isArdwiino(const QSerialPortInfo& serialPortInfo) -> bool {
    return serialPortInfo.vendorIdentifier() == SONY_VID || serialPortInfo.vendorIdentifier() == SWITCH_VID || (serialPortInfo.vendorIdentifier() == ARDWIINO_VID && serialPortInfo.productIdentifier() == ARDWIINO_PID);
}
ArdwiinoLookup* ArdwiinoLookup::instance = nullptr;
const board_t ArdwiinoLookup::empty =  {"","","",0,{},"","",0,"","", false};
const board_t ArdwiinoLookup::boards[5] = {
    {"uno-atmega16u2", "uno-usb","Arduino Uno",57600,{},"dfu","atmega16u2",16000000,"Arduino-COMBINED-dfu-usbserial-atmega16u2-Uno-Rev3.hex","images/uno.png",true},
    {"uno-at90usb82", "uno-usb","Arduino Uno",57600,{},"dfu","at90usb82",16000000,"UNO-dfu_and_usbserial_combined.hex","images/uno.png",true},
    {"uno", "uno-main","Arduino Uno",115200,{0x0043, 0x7523, 0x0001, 0xea60, 0x0243},"arduino","atmega328p",16000000,"","images/uno.png",true},
    {"micro", "micro","Arduino Pro Micro",57600,{0x9203, 0x9204,0x9205, 0x9206, 0x9207, 0x9208},"avr109","atmega32u4",8000000,"","images/micro.png", false},
    {"leonardo", "leonardo","Arduino Leonardo",57600,{0x0036, 0x8036, 0x800c},"avr109","atmega32u4",16000000,"","images/leonardo.png", false},
};

auto ArdwiinoLookup::findByBoard(const QString& board_name) -> const board_t {
    for (const auto& board: boards) {
        if (board.shortName == board_name) {
            return board;
        }
    }
    return empty;
}

auto ArdwiinoLookup::detectBoard(const QSerialPortInfo& serialPortInfo) -> const board_t {
    for (const auto& board: boards) {
        for (auto& pid : board.productIDs) {
            if (pid && pid == serialPortInfo.productIdentifier()) {
                return board;
            }
        }
    }
    return empty;
}
auto ArdwiinoLookup::getInstance() -> ArdwiinoLookup* {
    if (!ArdwiinoLookup::instance) {
        ArdwiinoLookup::instance = new ArdwiinoLookup();
    }
    return ArdwiinoLookup::instance;
}
