#pragma once
#include <QObject>
#include "submodules/Ardwiino/src/shared/config/defines.h"
#define ORIENTATION(type) type ## _TYPE=type
class MPU6050Orientations
{
    Q_GADGET
public:
    enum Value {
        ORIENTATION(POSITIVE_Z),
        ORIENTATION(NEGATIVE_Z),
        ORIENTATION(POSITIVE_Y),
        ORIENTATION(NEGATIVE_Y),
        ORIENTATION(POSITIVE_X),
        ORIENTATION(NEGATIVE_X),
        END=NEGATIVE_X_TYPE
    };

    Q_ENUM(Value)

    static QString toString(Value value) {
        switch(value) {
        case Value::POSITIVE_Z_TYPE: return "Rotation around Z, Positive";
        case Value::NEGATIVE_Z_TYPE: return "Rotation around Z, Negative";
        case Value::POSITIVE_Y_TYPE: return "Rotation around Y, Positive";
        case Value::NEGATIVE_Y_TYPE: return "Rotation around Y, Negative";
        case Value::POSITIVE_X_TYPE: return "Rotation around X, Positive";
        case Value::NEGATIVE_X_TYPE: return "Rotation around X, Negative";
        }
        return "Unknown rotation";
    }
};
