#include "ardwiinolookup.h"
#include "QDebug"
#include "wii_extensions.h"
#include "input_types.h"
ArdwiinoLookup::ArdwiinoLookup(QObject *parent):QObject(parent) {

}
QString ArdwiinoLookup::lookupExtension(uint8_t type, uint16_t device) {
    InputTypes::Value vtype = InputTypes::Value(type);
    if (vtype == InputTypes::WII_TYPE) {
      return QString("%1 %2").arg(InputTypes::toString(vtype)).arg(WiiExtensions::toString(WiiExtensions::Value(device)));
    }
    return InputTypes::toString(vtype);
}
QString ArdwiinoLookup::lookupType(uint8_t type) {
    return Controllers::toString(Controllers::Value(type));
}

//Ardwino PS3 Controllers use sony vids. No other sony controller should expose a serial port however, so we should be fine doing this.
bool ArdwiinoLookup::isArdwiino(const QSerialPortInfo& serialPortInfo) {
    return serialPortInfo.vendorIdentifier() == SONY_VID || (serialPortInfo.vendorIdentifier() == ARDWIINO_VID && serialPortInfo.productIdentifier() == ARDWIINO_PID);
}
ArdwiinoLookup* ArdwiinoLookup::instance = nullptr;
const board_t ArdwiinoLookup::empty =  {"","",0,{},"","",0,""};
const board_t ArdwiinoLookup::boards[4] = {
    {"uno-usb","Arduino Uno DFU",57600,{},"dfu","atmega16u2",16000000,"Arduino-COMBINED-dfu-usbserial-atmega16u2-Uno-Rev3"},
    {"uno-main","Arduino Uno",115200,{0x0043, 0x7523, 0x0001, 0xea60},"arduino","atmega328p",16000000,""},
    {"micro","Arduino Pro Micro",57600,{0x9203, 0x9204,0x9205, 0x9206, 0x9207, 0x9208},"avr109","atmega32u4",8000000,""},
    {"micro","Arduino Leonardo",57600,{0x0036, 0x8036, 0x800c},"avr109","atmega32u4",16000000,""},
};

const board_t ArdwiinoLookup::retriveDFUVariant(const board_t board) {
    if (board.hexFile.contains("-main")) {
        auto usb = board.hexFile.split("-")[0]+"-usb";
        for (const auto& board2: boards) {
            if (board2.hexFile == usb) {
                return board2;
            }
        }
    }
    return board;
}

bool ArdwiinoLookup::hasDFUVariant(const board_t board) {
    if (board.hexFile.contains("-main")) {
        auto usb = board.hexFile.split("-")[0]+"-usb";
        for (const auto& board2: boards) {
            if (board2.hexFile == usb) {
                return true;
            }
        }
    }
    return false;
}

const board_t* ArdwiinoLookup::detectBoard(const QSerialPortInfo& serialPortInfo) {
    for (const auto& board: boards) {
        for (auto& pid : board.productIDs) {
            if (pid && pid == serialPortInfo.productIdentifier()) {
                return &board;
            }
        }
    }
    return nullptr;
}
ArdwiinoLookup* ArdwiinoLookup::getInstance() {
    if (!ArdwiinoLookup::instance) {
        ArdwiinoLookup::instance = new ArdwiinoLookup();
    }
    return ArdwiinoLookup::instance;
}
QString ArdwiinoLookup::getControllerTypeName(Controllers::Value value) {
    return Controllers::toString(value);
}
