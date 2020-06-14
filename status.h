#pragma once
#include <QObject>

class Status : public QObject {
    Q_GADGET
   public:
    enum Value {
        NOT_PROGRAMMING,
        WAIT,
        WAIT_AVRDUDE,
        AVRDUDE,
        DFU_CONNECT_AVRDUDE,
        DFU_ERASE_AVRDUDE,
        DFU_FLASH_AVRDUDE,
        DFU_DISCONNECT_AVRDUDE,
        DFU_CONNECT_MAIN,
        DFU_ERASE_MAIN,
        DFU_FLASH_MAIN,
        DFU_DISCONNECT_MAIN,
        COMPLETE
    };

    Q_ENUM(Value)

    static QString toString(Value value) {
        switch (value) {
            case Value::NOT_PROGRAMMING:
                return "Not Programming";
            case Value::WAIT:
                return "Waiting";
            case Value::WAIT_AVRDUDE:
                return "Waiting for Avrdude";
            case Value::DFU_CONNECT_AVRDUDE:
            case Value::DFU_CONNECT_MAIN:
                return "Waiting for DFU Mode";
            case Value::AVRDUDE:
                return "Programming Main Controller";
            case Value::DFU_DISCONNECT_MAIN:
            case Value::DFU_DISCONNECT_AVRDUDE:
                return "Waiting for Reconnection";
            case Value::COMPLETE:
                return "Finished Programming";
            default:
                return "Programming USB Controller";
        }
    }
};
