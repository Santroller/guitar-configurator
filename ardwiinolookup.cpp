#include "ardwiinolookup.h"
#include "QDebug"
QString ArdwiinoLookup::lookupExtension(uint8_t type, uint16_t device) {
    switch (type) {
    case DIRECT:
        return "Wired ";
    case WII:
        switch (device) {
        case NUNCHUK:
            return "Wii Nunchuk ";
        case CLASSIC:
            return "Wii Classic Controller ";
        case CLASSIC_PRO:
            return "Wii Classic Controller Pro ";
        case UDRAW:
            return "Wii UDraw Tablet ";
        case DRAWSOME:
            return "Wii Drawsome Tablet ";
        case GUITAR:
            return "Wii Guitar ";
        case DRUMS:
            return "Wii Drums ";
        case TURNTABLE:
            return "Wii Turntable ";
        case TATACON:
            return "Wii Taiko no Tatsujin TaTaCon (Drum controller) ";
        case MOTION_PLUS:
            return "Wii Motion Plus ";
        case MOTION_PLUS_NUNCHUK:
            return "Wii Motion Plus (Nunchuk Passthrough) ";
        case MOTION_PLUS_CLASSIC:
            return "Wii Motion Plus (Classic Passthrough) ";
        case NO_DEVICE:
            return "No Wii Extension";
        default:
            return "Unknown Wii Extension";
        }
    }
    return "Unknown Ardwiino Controller";
}
QString ArdwiinoLookup::lookupType(uint8_t type) {
    switch (type) {
    case XINPUT_GAMEPAD_SUBTYPE:
        return "XInput Gamepad";
    case XINPUT_WHEEL_SUBTYPE:
        return "XInput Wheel";
    case XINPUT_ARCADE_STICK_SUBTYPE:
        return "XInput Arcade Stick";
    case XINPUT_FLIGHT_STICK_SUBTYPE:
        return "XInput Flight Stick";
    case XINPUT_DANCE_PAD_SUBTYPE:
        return "XInput Dance Pad";
    case XINPUT_GUITAR_SUBTYPE:
        return "XInput Guitar";
    case XINPUT_GUITAR_ALTERNATE_SUBTYPE:
        return "XInput Guitar (Alternate)";
    case XINPUT_DRUM_SUBTYPE:
        return "XInput Drums";
    case XINPUT_GUITAR_BASS_SUBTYPE:
        return "XInput Guitar (Bass)";
    case XINPUT_ARCADE_PAD_SUBTYPE:
        return "XInput Arcade Pad";
    case KEYBOARD_SUBTYPE:
        return "Keyboard";
    case SWITCH_GAMEPAD_SUBTYPE:
        return "Switch Gamepad";
    case PS3_GAMEPAD_SUBTYPE:
        return "PS3 Gamepad";
    case PS3_GUITAR_GH_SUBTYPE:
        return "PS3 Guitar Hero Guitar";
    case PS3_GUITAR_RB_SUBTYPE:
        return "PS3 Rockband Guitar";
    case PS3_DRUM_GH_SUBTYPE:
        return "PS3 Guitar Hero Drums";
    case PS3_DRUM_RB_SUBTYPE:
        return "PS3 Rock Band Drums";
    default:
        return "Unknown Type";
    }
}

//Ardwino PS3 Controllers use sony vids. No other sony controller should expose a serial port however, so we should be fine doing this.
bool ArdwiinoLookup::isArdwiino(const QSerialPortInfo* serialPortInfo) {
    return serialPortInfo->vendorIdentifier() == SONY_VID || (serialPortInfo->vendorIdentifier() == ARDWIINO_VID && serialPortInfo->productIdentifier() == ARDWIINO_PID);
}

const board_t ArdwiinoLookup::boards[4] = {
    {"uno-usb","Arduino Uno DFU",57600,{},"dfu","atmega16u2",16000000,true,{0x1e,0x94,0x89}},
    {"uno-main","Arduino Uno",115200,{0x0043, 0x7523, 0x0001, 0xea60},"avr109","atmega16u2",16000000,true,{0x1e, 0x95, 0x0F}},
    {"micro","Arduino Pro Micro",57600,{0x9203, 0x9204,0x9205, 0x9206},"avr109","atmega32u4",8000000,true,{0x1e, 0x95, 0x87}},
    {"micro","Arduino Leonardo",57600,{0x0036, 0x8036, 0x800c},"avr109","atmega32u4",16000000,true,{0x1e, 0x95, 0x87}},
};


const board_t* ArdwiinoLookup::detectBoard(const QSerialPortInfo* serialPortInfo) {
    for (const auto& board: boards) {
        for (auto& pid : board.productIDs) {
            if (pid && pid == serialPortInfo->productIdentifier()) {
                return &board;
            }
        }
    }
    return nullptr;
}
