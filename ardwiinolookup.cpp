#include "ardwiinolookup.h"

ArdwiinoLookup::ArdwiinoLookup()
{

}
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
