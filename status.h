#ifndef STATUS_H
#define STATUS_H
#include <QObject>

class Status
{
    Q_GADGET
public:
    explicit Status();
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
        return values[value];
    }
    const static QString values[8];
};

#endif // STATUS_H
