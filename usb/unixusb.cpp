#ifndef _WIN32
#include "usbdevice.h"

UsbDevice::UsbDevice(UsbDevice_t devt, QObject *parent) : QObject(parent), m_devt(devt)
{
    
}
bool UsbDevice::open() {
    // We need to claim the control interface in order to read and write from it.
    if (libusb_open(m_devt.dev, &libusb_handle) != LIBUSB_SUCCESS) {
        return false;
    }
    auto detach = libusb_detach_kernel_driver(libusb_handle,0);
    // We need this to either succeed, or for there not to be a kernel driver to detach, and in that case it will return not found.
    if (detach != LIBUSB_SUCCESS && detach != LIBUSB_ERROR_NOT_FOUND) {
        return false;
    }
    return libusb_claim_interface(libusb_handle,0) == LIBUSB_SUCCESS;
}

void UsbDevice::close() {
    if (libusb_handle) {
        libusb_attach_kernel_driver(libusb_handle, 0);
        libusb_close(libusb_handle);
    }
}

int UsbDevice::write(int id, QByteArray data) {
    return libusb_control_transfer(libusb_handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_ENDPOINT_OUT, REQ_HID_SET_REPORT, id, 0, (unsigned char*)data.data(), data.length(), 0);
}

QByteArray UsbDevice::read(int id) {
    QByteArray data(64, '\0');
    auto len = libusb_control_transfer(libusb_handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_ENDPOINT_IN, REQ_HID_GET_REPORT, id, 0, (unsigned char*)data.data(), data.length(), 0);
    if (len < 0) {
        qDebug() << QString::fromUtf8(libusb_error_name(len));
    }
    data.resize(len);
    // All data being sent from the controller starts with a header byte that we can just remove.
    data.remove(0,1);
    return data;
}
#endif