#pragma once

#include <QObject>

#include "devices/device.h"
extern "C" {
#ifdef Q_OS_WIN
#include <Windows.h>
#include <fileapi.h>
#include <winusb.h>
#endif
#if defined(Q_OS_MACOS)
#include <libusb.h>
#else
#include <libusb-1.0/libusb.h>
#endif
}


#define REQ_HID_GET_REPORT 1
#define REQ_HID_SET_REPORT 9
class UsbDevice : public QObject {
    Q_OBJECT
   public:
    explicit UsbDevice(UsbDevice_t* devt, QObject *parent = nullptr);
    bool open();
    void close();
    int write(int id, QByteArray data);
    QByteArray read(int id);
   signals:

   private:
    UsbDevice_t* m_devt;
#ifdef Q_OS_WIN
    WINUSB_INTERFACE_HANDLE winusb_handle;
    HANDLE device_handle;
#else
    libusb_device_handle* libusb_handle;
#endif
};
