#pragma once
#include <QObject>

class Status : public QObject
{
    Q_GADGET
public:
    enum Value {
        NOT_PROGRAMMING,
        WAIT,
        AVRDUDE,
        DFU_CONNECT,
        DFU_ERASE,
        DFU_FLASH,
        DFU_DISCONNECT,
        COMPLETE
    };

    Q_ENUM(Value)

    static QString toString(Value value) {
        switch(value) {
        case Value::NOT_PROGRAMMING: return "Not Programming";
        case Value::WAIT: return "Waiting";
        case Value::DFU_CONNECT: return "Waiting for DFU Mode";
        case Value::AVRDUDE: return "Programming Main Controller";
        case Value::DFU_DISCONNECT: return "Waiting for Reconnection";
        case Value::COMPLETE: return "Finished Programming";
        default: return "Programming USB Controller";
        }
    }
};
