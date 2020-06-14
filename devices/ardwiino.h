#pragma once
#include <math.h>

#include <QDebug>
#include <QJSValue>
#include <QObject>
#include <QQueue>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QUsbInfo>
#include <QUsbDevice>
#include <hidapi/hidapi.h>

#include "device.h"
class Ardwiino : public Device {
    Q_OBJECT
   public:
    explicit Ardwiino(QUsbDevice::Id usbId, wchar_t* serialNumber, unsigned short version, QObject* parent = nullptr);
    QString getDescription();
    bool isReady();
    virtual void close();
    virtual void open();
   signals:
    void descriptionChanged();
    void readyChanged();
    
   protected:
    board_t m_board;
    QString m_processor;

   private:
    QUsbDevice::Id m_usbId;
    wchar_t* m_serialNumber;
    hid_device* m_hiddev;
    unsigned short m_version;
    QByteArray readData(int id);
    inline virtual bool isEqual(const Device& other) const {
        auto& otherD = static_cast<const Ardwiino&>(other);
        return m_serialNumber == otherD.m_serialNumber && m_version == otherD.m_version;
    }
};
