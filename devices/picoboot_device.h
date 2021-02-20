#pragma once
#include <math.h>

#include <QDebug>
#include <QJSValue>
#include <QObject>
#include <QQueue>
#include <QSerialPort>
#include <QFile>
#include <QSerialPortInfo>

#include "device.h"
class PicobootDevice : public Device {
    Q_OBJECT
   public:
    explicit PicobootDevice(UsbDevice_t devt, QObject* parent = nullptr);
    QString getDescription();
    bool isReady();
    virtual void close();
    virtual bool open();
    virtual void bootloader();
    void program(QFile* firmware, std::function<void(long, long, int, int)> progress);
    inline virtual bool isConfigurable() {
        return false;
    }
   signals:
    void descriptionChanged();
    void readyChanged();

   private:
    inline virtual bool isEqual(const Device& other) const {
        return true;
    }
};

template <typename T> struct raw_type_mapping {
};
#define SAFE_MAPPING(type) template<> struct raw_type_mapping<type> { typedef type access_type; }
SAFE_MAPPING(uint8_t);
SAFE_MAPPING(char);
SAFE_MAPPING(uint16_t);
SAFE_MAPPING(uint32_t);
#define ERROR_ARGS -1
#define ERROR_FORMAT -2
#define ERROR_INCOMPATIBLE -3
#define ERROR_READ_FAILED -4
#define ERROR_WRITE_FAILED -5
#define ERROR_USB -6
#define ERROR_NO_DEVICE -7
#define ERROR_NOT_POSSIBLE -8
#define ERROR_CONNECTION -9
#define ERROR_CANCELLED -10
#define ERROR_VERIFICATION_FAILED -11
#define ERROR_UNKNOWN -99
#define BOOTROM_MAGIC 0x01754d
#define BOOTROM_MAGIC_ADDR 0x00000010