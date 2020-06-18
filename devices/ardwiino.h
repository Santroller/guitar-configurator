#pragma once
#include <math.h>

#include <QDebug>
#include <QJSValue>
#include <QObject>
#include <QQueue>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <hidapi.h>

#include "device.h"
class Ardwiino : public Device {
    Q_OBJECT
   public:
    explicit Ardwiino(struct hid_device_info *usbDeviceId, QObject* parent = nullptr);
    QString getDescription();
    bool isReady();
    virtual void close();
    virtual bool open();
   signals:
    void descriptionChanged();
    void readyChanged();

   protected:
    board_t m_board;
    QString m_processor;

   private:
    struct hid_device_info *m_usbId;
    hid_device* m_hiddev;
    QByteArray readData(int id);
    QString m_serialNum;
    Configuration_t configuration;
    inline virtual bool isEqual(const Device& other) const {
        auto& otherD = static_cast<const Ardwiino&>(other);
        return m_serialNum == otherD.m_serialNum;
    }
};
