#pragma once
#include <QObject>
#include "submodules/Ardwiino/src/shared/config/defines.h"
class JoyTypes
{
    Q_GADGET
public:
    enum Value {
        JOY_TYPE_NONE,
        JOY_TYPE_JOYSTICK,
        JOY_TYPE_DPAD,
        END=JOY_TYPE_DPAD
    };

    Q_ENUM(Value)

    static QString toString(Value value) {
        switch(value) {
        case Value::JOY_TYPE_NONE: return "No Joystick / DPad";
        case Value::JOY_TYPE_JOYSTICK: return "Joystick";
        case Value::JOY_TYPE_DPAD: return "DPad";
        }
        return "No Joystick / DPad";
    }
};
