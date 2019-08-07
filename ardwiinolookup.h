#ifndef ARDWIINOLOOKUP_H
#define ARDWIINOLOOKUP_H
#include <QObject>
#include <QTimer>
#include <QSerialPortInfo>
#include <QList>
#include "submodules/Ardwiino/src/shared/input/input_wii_ext.h"
#include "submodules/Ardwiino/src/shared/config/config.h"
#include "submodules/Ardwiino/src/shared/config/defaults.h"
#include "submodules/Ardwiino/src/shared/controller/controller.h"

class ArdwiinoLookup
{
public:
    ArdwiinoLookup();
    static QString lookupExtension(uint8_t type, uint16_t device);
    static QString lookupType(uint8_t type);
};

#endif // ARDWIINOLOOKUP_H
