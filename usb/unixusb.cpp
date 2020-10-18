#ifndef Q_OS_WIN
#include "usbdevice.h"

UsbDevice::UsbDevice(UsbDevice_t devt, QObject *parent) : QObject(parent), m_devt(devt)
{
    
}
bool UsbDevice::open() {
    // We need to claim the control interface in order to read and write from it.
    if (libusb_open(m_devt.dev, &libusb_handle) != 0) {
        return false;
    }
    if (libusb_detach_kernel_driver(libusb_handle,0) != 0) {
        return false;
    }
    return libusb_claim_interface(libusb_handle,0) == 0;
}

void UsbDevice::close() {
    if (libusb_handle) {
        libusb_close(libusb_handle);
    }
}

int UsbDevice::write(int id, QByteArray data) {
    return libusb_control_transfer(libusb_handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_ENDPOINT_OUT, REQ_HID_SET_REPORT, id, 0, (unsigned char*)data.data(), data.length(), 0);
}

QByteArray UsbDevice::read(int id) {
    QByteArray data(64, '\0');
    auto err = libusb_control_transfer(libusb_handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_ENDPOINT_IN, REQ_HID_GET_REPORT, id, 0, (unsigned char*)data.data(), data.length(), 0);
    qDebug() << QString::fromUtf8(libusb_error_name(err));
    return data;
}
#endif