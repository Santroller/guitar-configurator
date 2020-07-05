#pragma once
#include <math.h>

#include <QDebug>
#include <QJSValue>
#include <QObject>
#include <QQueue>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "ardwiino_defines.h"
#include "ardwiinolookup.h"
#include "serialdevice.h"
#include "submodules/Ardwiino/src/shared/output/serial_commands.h"
class OutdatedArdwiino : public SerialDevice {
    Q_OBJECT
   public:
    explicit OutdatedArdwiino(const QSerialPortInfo &serialPortInfo, QObject *parent = nullptr);
    QString getDescription();
    bool isReady();
    virtual bool open();
    virtual void close();
    virtual void bootloader();
    inline virtual bool isConfigurable() {
        return false;
    }
    inline virtual bool isEqual(const Device& other) const {
        return m_serialPort->portName() == static_cast<const OutdatedArdwiino&>(other).m_serialPort->portName();
    }
    bool m_isOldAPI;
};
