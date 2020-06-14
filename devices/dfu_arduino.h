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

#include "device.h"
class DfuArduino : public Device {
    Q_OBJECT
   public:
    explicit DfuArduino(QString processor, QUsbDevice::Id usbId, QObject* parent = nullptr);
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
    inline virtual bool isEqual(const Device& other) const {
        auto& otherD = static_cast<const DfuArduino&>(other);
        return m_processor == otherD.m_processor && m_usbId == otherD.m_usbId;
    }
};
