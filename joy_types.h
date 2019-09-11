#pragma once
#include <QObject>
#include "submodules/Ardwiino/src/shared/config/defines.h"
#define JOYTYPE(type) type ## _JOY=type
class JoyTypes
{
    Q_GADGET
public:
    enum Value {
        JOYTYPE(NONE),
        JOYTYPE(DPAD),
        JOYTYPE(JOY),
        END=JOY_JOY
    };

    Q_ENUM(Value)

    static QString toString(Value value) {
        switch(value) {
        case Value::NONE_JOY: return "No Joystick";
        case Value::DPAD_JOY: return "DPad";
        case Value::JOY_JOY: return "Joystick";
        }
    }
};
