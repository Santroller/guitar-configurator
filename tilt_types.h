#pragma once
#include <QObject>
#include "submodules/Ardwiino/src/shared/config/defines.h"
#define TILTTYPE(type) type ## _SENSOR=type
class TiltTypes
{
    Q_GADGET
public:
    enum Value {
        TILTTYPE(NONE),
        TILTTYPE(MPU_6050),
        TILTTYPE(GRAVITY),
        TILTTYPE(ANALOGUE),
        END=ANALOGUE_SENSOR
    };

    Q_ENUM(Value)

    static QString toString(Value value) {
        switch(value) {
        case Value::NONE_SENSOR: return "No Sensor";
        case Value::MPU_6050_SENSOR: return "MPU 6050 Based Sensor";
        case Value::GRAVITY_SENSOR: return "Digital Sensor";
        case Value::ANALOGUE_SENSOR: return "Analogue Sensor";
        }
    }
};
