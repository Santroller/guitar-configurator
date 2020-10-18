#ifdef _WIN32
#include "usbdevice.h"

UsbDevice::UsbDevice(UsbDevice_t devt, QObject *parent) : QObject(parent), m_devt(devt)
{
    
}
bool UsbDevice::open() {
    device_handle = CreateFile(m_devt.hidPath.toUtf8(),
                                              GENERIC_WRITE | GENERIC_READ,
                                              FILE_SHARE_WRITE | FILE_SHARE_READ,
                                              NULL,
                                              OPEN_EXISTING,
                                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                              NULL);
    if(!WinUsb_Initialize(device_handle,  &winusb_handle)) {
        auto err = GetLastError();
        qDebug() << err << "Winusb Failure!";
        return false;
    }
    return true;
}

void UsbDevice::close() {
    if (winusb_handle) {
        WinUsb_Free(winusb_handle);
        CloseHandle(device_handle);
    }
}

int UsbDevice::write(int id, QByteArray data) {
    BOOL bResult = TRUE;
    UCHAR* d = (UCHAR*)data.data();
    WINUSB_SETUP_PACKET SetupPacket;
    ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
    ULONG cbSent = 0;
    BM_REQUEST_TYPE rt;
    rt.s.Dir = BMREQUEST_HOST_TO_DEVICE;
    rt.s.Reserved = 0;
    rt.s.Type = BMREQUEST_CLASS;
    rt.s.Recipient = BMREQUEST_TO_INTERFACE;
    SetupPacket.RequestType = rt.B;
    SetupPacket.Request = REQ_HID_SET_REPORT;
    SetupPacket.Value = id;
    SetupPacket.Index = 0;
    SetupPacket.Length = sizeof(d) * sizeof(UCHAR);

    bResult = WinUsb_ControlTransfer(winusb_handle, SetupPacket, d, sizeof(d) * sizeof(UCHAR), &cbSent, 0);
    if (!bResult) {
        return -(int)GetLastError();
    }
    return cbSent;
}

QByteArray UsbDevice::read(int id) {
    BOOL bResult = TRUE;
    QByteArray data(64, '\0');
    WINUSB_SETUP_PACKET SetupPacket;
    ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
    ULONG cbSent = 0;
    BM_REQUEST_TYPE rt;
    rt.s.Dir = BMREQUEST_DEVICE_TO_HOST;
    rt.s.Reserved = 0;
    rt.s.Type = BMREQUEST_CLASS;
    rt.s.Recipient = BMREQUEST_TO_INTERFACE;
    SetupPacket.RequestType = rt.B;
    SetupPacket.Request = REQ_HID_GET_REPORT;
    
    SetupPacket.Value = id;
    SetupPacket.Index = 0;
    SetupPacket.Length = data.length();

    bResult = WinUsb_ControlTransfer(winusb_handle, SetupPacket, (UCHAR*)data.data(), data.length(), &cbSent, 0);
    if (!bResult)
    {
       auto err = GetLastError();
    //    TODO: we should handle errors somehow, maybe we pass in the qbytearray and return the bytes sent?
    }
    return data;
}
#endif
