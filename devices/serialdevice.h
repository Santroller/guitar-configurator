#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "device.h"

class SerialDevice : public Device {
    Q_OBJECT
   public:
    explicit SerialDevice(const QSerialPortInfo& serialInfo, QObject* parent = nullptr);
    virtual void close();
    inline virtual bool isConfigurable() {
        return false;
    }
    QString getLocation() const {
        return m_location;
    }
    
   protected:
    QSerialPort* m_serialPort;
    QString m_location;
    QByteArray readWrite(QByteArray data);
   private:
    inline virtual bool isEqual(const Device& other) const {
        return m_serialPort->portName() == static_cast<const SerialDevice&>(other).m_serialPort->portName();
    }
};

#endif  // SERIALDEVICE_H
