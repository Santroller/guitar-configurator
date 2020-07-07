#pragma once
#include <math.h>

#include <QDebug>
#include <QJSValue>
#include <QObject>
#include <QQueue>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "ardwiino_defines.h"
#include "board.h"
#include "submodules/Ardwiino/src/shared/output/serial_commands.h"
#ifdef Q_OS_MACOS
#include <libusb.h>
#else
#include <libusb-1.0/libusb.h>
#endif
typedef struct UsbDevice_t {
    int bus;
    int port;
    int vid;
    int pid;
    QString serial;
    bool operator==(const UsbDevice_t& other) {
        return port == other.port && bus == other.bus;
    }
} UsbDevice_t;
class Device : public QObject {
    Q_PROPERTY(QString description READ getDescription NOTIFY descriptionChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)
    Q_PROPERTY(bool configurable READ isConfigurable NOTIFY configurableChanged)
    Q_PROPERTY(QString boardImage READ getBoardImage NOTIFY boardImageChanged)
    Q_OBJECT
   public:
    explicit Device(UsbDevice_t m_deviceID, QObject* parent = nullptr);
    virtual QString getDescription() = 0;
    virtual bool isReady() = 0;
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual void bootloader() = 0;
    virtual bool isConfigurable() = 0;
    void setBoardType(QString board);
    inline bool operator==(const Device& other) {
        return typeid(*this) == typeid(other) && m_deviceID == other.m_deviceID && isEqual(other);
    }
   public slots:
    inline QString getBoardImage() const {
        return m_board.image;
    }
    inline void readDescription() {
        emit descriptionChanged();
    }
   signals:
    void descriptionChanged();
    void readyChanged();
    void configurableChanged();
    void boardImageChanged();

   protected:
    board_t m_board;
    UsbDevice_t m_deviceID;

   private:
    virtual bool isEqual(const Device& other) const = 0;
};
