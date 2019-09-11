#pragma once
#include <QObject>

class Status
{
    Q_GADGET
public:
    enum Value {
        WAIT_AVRDUDE,
        AVRDUDE,
        DFU_CONNECT,
        DFU_ERASE,
        DFU_EEPROM,
        DFU_FLASH,
        DFU_DISCONNECT,
        COMPLETE
    };

    Q_ENUM(Value)

    static QString toString(Value value) {
        switch(value) {
        case Value::WAIT_AVRDUDE: return "Waiting";
        case Value::AVRDUDE: return "Programming Main Controller";
        case Value::DFU_CONNECT: return "Waiting for DFU Mode";
        case Value::DFU_DISCONNECT: return "Waiting for Reconnection";
        case Value::COMPLETE: return "Finished Programming";
        default: return "Programming USB Controller";
        }
    }
};
