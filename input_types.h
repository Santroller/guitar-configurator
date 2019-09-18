#pragma once
#include <QObject>
#include "submodules/Ardwiino/src/shared/config/defines.h"
#define INPUTTYPE(type) type ## _TYPE=type
class InputTypes
{
    Q_GADGET
public:
    enum Value {
        INPUTTYPE(WII),
        INPUTTYPE(DIRECT),
        END=DIRECT_TYPE
    };

    Q_ENUM(Value)

    static QString toString(Value value) {
        switch(value) {
        case Value::WII_TYPE: return "Wii Extension Adaptor";
        case Value::DIRECT_TYPE: return "Directly Wired Controller";
        }
        return "Unknown Controller Type";
    }
};
