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
#include "submodules/Ardwiino/src/shared/output/serial_commands.h"
#include "device.h"
class NullDevice : public Device {
    Q_OBJECT
   public:
    explicit NullDevice(QObject *parent = nullptr);
    virtual QString getDescription();
    virtual bool isReady();
    virtual void close();
    virtual bool open();
    virtual void bootloader();
   signals:
    void descriptionChanged();
    void readyChanged();
   private:
    inline virtual bool isEqual(const Device& other) const {
        return true;
    };
};
