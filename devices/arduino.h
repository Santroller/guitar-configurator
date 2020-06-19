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
class Arduino : public SerialDevice {
    Q_OBJECT
   public:
    explicit Arduino(const QSerialPortInfo &serialPortInfo, QObject *parent = nullptr);
    QString getDescription();
    bool isReady();
    QString versionCmd;
    QString freqCmd;
    virtual void close();
    virtual bool open();
    virtual void bootloader();
    inline virtual bool isEqual(const Device& other) const {
        return m_serialPort->portName() == static_cast<const Arduino&>(other).m_serialPort->portName();
    }
};
