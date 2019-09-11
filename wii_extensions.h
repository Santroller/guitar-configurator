#pragma once
#include <QObject>
#include "submodules/Ardwiino/src/shared/input/input_wii_ext.h"
#define EXT(type) type ## _EXT=type
class WiiExtensions
{
    Q_GADGET
public:
    enum Value {
        EXT(NUNCHUK),
        EXT(CLASSIC),
        EXT(CLASSIC_PRO),
        EXT(UDRAW),
        EXT(DRAWSOME),
        EXT(GUITAR),
        EXT(DRUMS),
        EXT(TURNTABLE),
        EXT(TATACON),
        EXT(MOTION_PLUS),
        EXT(MOTION_PLUS_NUNCHUK),
        EXT(MOTION_PLUS_CLASSIC),
        EXT(NO_DEVICE),
        END=NO_DEVICE_EXT
    };

    Q_ENUM(Value)

    static QString toString(Value value) {
        switch(value) {
        case NUNCHUK_EXT:
            return "Nunchuk";
        case CLASSIC_EXT:
            return "Classic Controller";
        case CLASSIC_PRO_EXT:
            return "Classic Controller Pro";
        case UDRAW_EXT:
            return "UDraw Tablet";
        case DRAWSOME_EXT:
            return "Drawsome Tablet";
        case GUITAR_EXT:
            return "Guitar";
        case DRUMS_EXT:
            return "Drums";
        case TURNTABLE_EXT:
            return "Turntable";
        case TATACON_EXT:
            return "Taiko no Tatsujin TaTaCon (Drum controller)";
        case MOTION_PLUS_EXT:
            return "Motion Plus ";
        case MOTION_PLUS_NUNCHUK_EXT:
            return "Motion Plus (Nunchuk Passthrough)";
        case MOTION_PLUS_CLASSIC_EXT:
            return "Motion Plus (Classic Passthrough)";
        case NO_DEVICE_EXT:
            return "No Wii Extension";
        }
        return "Unknown Extension";
    }
};
