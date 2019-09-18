#pragma once
#include <QObject>
#include "submodules/Ardwiino/src/shared/config/defines.h"
#define SUBTYPE(type) type=type ## _SUBTYPE
class Controllers
{
    Q_GADGET
public:
    enum Value {
        SUBTYPE(XINPUT_GAMEPAD),
        SUBTYPE(XINPUT_WHEEL),
        SUBTYPE(XINPUT_ARCADE_STICK),
        SUBTYPE(XINPUT_FLIGHT_STICK),
        SUBTYPE(XINPUT_DANCE_PAD),
        SUBTYPE(XINPUT_GUITAR),
        SUBTYPE(XINPUT_GUITAR_ALTERNATE),
        SUBTYPE(XINPUT_DRUM),
        SUBTYPE(XINPUT_GUITAR_BASS),
        SUBTYPE(XINPUT_ARCADE_PAD),
        SUBTYPE(KEYBOARD),
        SUBTYPE(SWITCH_GAMEPAD),
        SUBTYPE(PS3_GAMEPAD),
        SUBTYPE(PS3_GUITAR_GH),
        SUBTYPE(PS3_GUITAR_RB),
        SUBTYPE(PS3_DRUM_GH),
        SUBTYPE(PS3_DRUM_RB),
        END=PS3_DRUM_RB
    };

    Q_ENUM(Value)
    static QString getImage(Value value) {
        switch(value) {
        case XINPUT_GAMEPAD:
            return "images/xbox.png";
        case XINPUT_WHEEL:
            return "images/xbox.png";
        case XINPUT_ARCADE_STICK:
            return "images/arcadestick.png";
        case XINPUT_FLIGHT_STICK:
            return "images/flightstick.png";
        case XINPUT_DANCE_PAD:
            return "images/ddr.png";
        case XINPUT_GUITAR:
        case XINPUT_GUITAR_ALTERNATE:
        case XINPUT_GUITAR_BASS:
            return "images/guitar.png";
        case XINPUT_DRUM:
            return "images/rb-drums.png";
        case XINPUT_ARCADE_PAD:
            return "images/arcadestick.png";
        case KEYBOARD:
            return "images/keyboard.png";
        case SWITCH_GAMEPAD:
            return "images/controller.png";
        case PS3_GAMEPAD:
            return "images/controller.png";
        case PS3_GUITAR_GH:
        case PS3_GUITAR_RB:
            return "images/guitar.png";
        case PS3_DRUM_GH:
        case PS3_DRUM_RB:
            return "images/rb-drums.png";
        }
        return "images/controller.png";
    }
    static QString toString(Value value) {
        switch(value) {
        case XINPUT_GAMEPAD:
            return "XInput Gamepad";
        case XINPUT_WHEEL:
            return "XInput Wheel";
        case XINPUT_ARCADE_STICK:
            return "XInput Arcade Stick";
        case XINPUT_FLIGHT_STICK:
            return "XInput Flight Stick";
        case XINPUT_DANCE_PAD:
            return "XInput Dance Pad";
        case XINPUT_GUITAR:
            return "XInput Guitar";
        case XINPUT_GUITAR_ALTERNATE:
            return "XInput Guitar (Alternate)";
        case XINPUT_DRUM:
            return "XInput Drums";
        case XINPUT_GUITAR_BASS:
            return "XInput Guitar (Bass)";
        case XINPUT_ARCADE_PAD:
            return "XInput Arcade Pad";
        case KEYBOARD:
            return "Keyboard";
        case SWITCH_GAMEPAD:
            return "Switch Gamepad";
        case PS3_GAMEPAD:
            return "PS3 Gamepad";
        case PS3_GUITAR_GH:
            return "PS3 Guitar Hero Guitar";
        case PS3_GUITAR_RB:
            return "PS3 Rockband Guitar";
        case PS3_DRUM_GH:
            return "PS3 Guitar Hero Drums";
        case PS3_DRUM_RB:
            return "PS3 Rock Band Drums";
        }
        return "Unknown Controller";
    }
};
