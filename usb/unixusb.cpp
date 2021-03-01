#ifndef _WIN32
#include "usbdevice.h"

UsbDevice::UsbDevice(UsbDevice_t* devt, QObject *parent) : QObject(parent), m_devt(devt)
{
    
}
bool UsbDevice::open() {
    if (libusb_open(m_devt->dev, &libusb_handle) != LIBUSB_SUCCESS) {
        return false;
    }
    // There is no kernel driver for the config interface so we don't have to do this
    // auto detach = libusb_detach_kernel_driver(libusb_handle,2);
    // We need this to either succeed, or for there not to be a kernel driver to detach, and in that case it will return not found.
    // if (detach != LIBUSB_SUCCESS && detach != LIBUSB_ERROR_NOT_FOUND) {
    //     return false;
    // }
    // We need to claim the config interface in order to read and write from it.
    return libusb_claim_interface(libusb_handle,2) == LIBUSB_SUCCESS;
}

void UsbDevice::close() {
    if (libusb_handle) {
        libusb_attach_kernel_driver(libusb_handle, 2);
        libusb_close(libusb_handle);
    }
}

int UsbDevice::write(int id, QByteArray data) {
    return libusb_control_transfer(libusb_handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_ENDPOINT_OUT, REQ_HID_SET_REPORT, id, 2, (unsigned char*)data.data(), data.length(), 0);
}

QByteArray UsbDevice::read(int id) {
    QByteArray data(64, '\0');
    auto len = libusb_control_transfer(libusb_handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_ENDPOINT_IN, REQ_HID_GET_REPORT, id, 2, (unsigned char*)data.data(), data.length(), 0);
    if (len < 0) {
        qDebug() << QString::fromUtf8(libusb_error_name(len));
    }
    data.resize(len);
    return data;
}
#endif
