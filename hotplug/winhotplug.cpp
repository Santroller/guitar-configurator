#include "winhotplug.h"
#ifdef Q_OS_WIN

#include <QDebug>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "hid.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "Cfgmgr32.lib")
#pragma comment(lib, "OneCoreUAP.lib")

#include <Cfgmgr32.h>
#include <Dbt.h>
#include <Windows.h>
#include <setupapi.h>

#undef DEFINE_GUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    static const GUID name = {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}
DEFINE_GUID(GUID_DEVINTERFACE_ARDWIINO,
            0xdf59037d, 0x7c92, 0x4155, 0xac, 0x12, 0x7d, 0x70, 0x0a, 0x31, 0x3d, 0x78);
#include <Usbiodef.h>

#include <QRegularExpression>

bool operator==(const QSerialPortInfo& lhs, const QSerialPortInfo& rhs) {
    return lhs.manufacturer() == rhs.manufacturer() && lhs.serialNumber() == rhs.serialNumber();
}

QList<QString> buses;
void lookupUSBInfo(bool isArdwiino, wchar_t* dbcc_name, HWND hwnd, UsbDevice_t* device) {
    HDEVINFO handle = SetupDiCreateDeviceInfoList(NULL, hwnd);
    SP_DEVICE_INTERFACE_DATA data;
    SP_DEVINFO_DATA infoData;
    wchar_t szDeviceInstanceID[MAX_DEVICE_ID_LEN];
    data.cbSize = sizeof(data);
    infoData.cbSize = sizeof(infoData);
    if (SetupDiOpenDeviceInterfaceW(handle, dbcc_name, 0, &data)) {
        int memberIndex = 0;
        while (SetupDiEnumDeviceInfo(handle, memberIndex, &infoData)) {
            memberIndex++;
            if (isArdwiino) {
                device->hidPath = QString::fromWCharArray(dbcc_name);
                wchar_t* ifaceList;
                CONFIGRET cres;
                while (true) {
                    cres = CM_Get_Parent(&infoData.DevInst, infoData.DevInst, NULL);
                    if (cres != CR_SUCCESS) {
                        break;
                    }
                    CM_Get_Device_IDW(infoData.DevInst, szDeviceInstanceID, MAX_DEVICE_ID_LEN, 0);
                    // Get list size
                    ULONG ifaceListSize = 0;
                    GUID dev = GUID_DEVINTERFACE_USB_DEVICE;
                    cres = CM_Get_Device_Interface_List_SizeW(&ifaceListSize, &dev, szDeviceInstanceID, CM_GET_DEVICE_INTERFACE_LIST_PRESENT | CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES);
                    if (cres != CR_SUCCESS) {
                        qDebug() << "ERROR -12";
                        break;
                    }
                    // Allocate memory for the list
                    ifaceList = new wchar_t[ifaceListSize];

                    // Populate the list
                    cres = CM_Get_Device_Interface_ListW(&dev, szDeviceInstanceID, ifaceList, ifaceListSize, CM_GET_DEVICE_INTERFACE_LIST_PRESENT | CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES);
                    if (cres != CR_SUCCESS) {
                        delete[] ifaceList;
                        qDebug() << "ERROR -13";
                        break;
                    }
                    if (!QString::fromWCharArray(ifaceList).isEmpty()) {
                        break;
                    }
                }
                if (ifaceList) {
                    lookupUSBInfo(false, ifaceList, hwnd, device);
                }
            } else {
                char hardwareID[1024];

                DWORD size, port_nr;
                SetupDiGetDeviceRegistryPropertyA(handle, &infoData, SPDRP_ADDRESS, NULL, (PBYTE)&port_nr, sizeof(port_nr), &size);
                SetupDiGetDeviceRegistryPropertyA(handle, &infoData, SPDRP_HARDWAREID, NULL, (PBYTE)&hardwareID, sizeof(hardwareID), &size);
                QRegularExpression idMatch(".+VID_(\\w{4})&PID_(\\w{4}).+");
                auto match = idMatch.globalMatch(QString::fromUtf8(hardwareID)).next();
                device->vid = match.captured(1).toInt(NULL, 16);
                device->pid = match.captured(2).toInt(NULL, 16);
                device->port = port_nr;
                CM_Get_Device_IDW(infoData.DevInst, szDeviceInstanceID, MAX_DEVICE_ID_LEN, 0);
                qDebug() << QString::fromWCharArray(szDeviceInstanceID);
                device->serial = QString::fromWCharArray(szDeviceInstanceID).split("\\")[2];
                while (infoData.DevInst) {
                    CM_Get_Parent(&infoData.DevInst, infoData.DevInst, NULL);
                    CM_Get_Device_IDW(infoData.DevInst, szDeviceInstanceID, MAX_DEVICE_ID_LEN, 0);
                    int bus_number = buses.indexOf(QString::fromWCharArray(szDeviceInstanceID));
                    if (bus_number != -1) {
                        device->bus = bus_number + 1;
                        break;
                    }
                }
            }
        }
        SetupDiDeleteDeviceInterfaceData(handle, &data);
    }
    SetupDiDestroyDeviceInfoList(handle);
}
WinHotplug::WinHotplug(PortScanner* scanner) : scanner(scanner) {
    for (auto a : QSerialPortInfo::availablePorts()) {
        m_port_list.push_back(a);
        scanner->serialDeviceDetected(a);
    }
    DWORD _index = 0;
    HDEVINFO handle = SetupDiGetClassDevsW(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    SP_DEVICE_INTERFACE_DATA data;
    SP_DEVINFO_DATA infoData;
    DWORD size;
    data.cbSize = sizeof(data);
    infoData.cbSize = sizeof(SP_DEVINFO_DATA);
    //First, we need to loop trough all usb host controllers and assign them an id in order, as this is how libusb gets its bus number
    //Doing this allows us to support detecting multiple unos in dfu mode at once.
    for (;;) {
        if (!SetupDiEnumDeviceInfo(handle, _index, &infoData)) {
            if (GetLastError() != ERROR_NO_MORE_ITEMS) {
                return;
            }
            break;
        }
        (_index)++;

        if (!SetupDiEnumDeviceInterfaces(handle, &infoData, &GUID_DEVINTERFACE_USB_HOST_CONTROLLER, 0, &data)) {
            if (GetLastError() != ERROR_NO_MORE_ITEMS) {
                qDebug() << "Could not obtain interface data for %s devInst %lX: %s";
                return;
            }
            continue;
        }

        SetupDiDeleteDeviceInterfaceData(handle, &data);
        wchar_t szDeviceInstanceID[MAX_DEVICE_ID_LEN];
        CM_Get_Device_IDW(infoData.DevInst, szDeviceInstanceID, MAX_DEVICE_ID_LEN, 0);
        buses << QString::fromWCharArray(szDeviceInstanceID);
        // Device does not have an interface matching this GUID, skip
    }
    _index = 0;
    for (;;) {
        if (!SetupDiEnumDeviceInfo(handle, _index, &infoData)) {
            if (GetLastError() != ERROR_NO_MORE_ITEMS) {
                SetupDiDeleteDeviceInterfaceData(handle, &data);
                break;
            }
            break;
        }
        (_index)++;
        if (SetupDiEnumDeviceInterfaces(handle, &infoData, &GUID_DEVINTERFACE_ARDWIINO, 0, &data) || SetupDiEnumDeviceInterfaces(handle, &infoData, &GUID_DEVINTERFACE_USB_DEVICE, 0, &data)) {
            if (!SetupDiGetDeviceInterfaceDetailA(handle, &data, NULL, 0, &size, NULL)) {
                // The dummy call should fail with ERROR_INSUFFICIENT_BUFFER
                if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
                    qDebug() << "expected insuff";
                    continue;
                }
            }
            size *= sizeof(wchar_t);
            PSP_DEVICE_INTERFACE_DETAIL_DATA_W dev_interface_details = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)malloc(size);
            dev_interface_details->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
            if (!SetupDiGetDeviceInterfaceDetailW(handle, &data, dev_interface_details, size, NULL, NULL)) {
                DWORD lastError = GetLastError();
                qDebug() << "unable to get path" << lastError;
                continue;
            }
            QString path = QString::fromWCharArray(dev_interface_details->DevicePath);
            UsbDevice_t dev = {};
            lookupUSBInfo(data.InterfaceClassGuid == GUID_DEVINTERFACE_ARDWIINO, dev_interface_details->DevicePath, NULL, &dev);
            free(dev_interface_details);
            SetupDiDeleteDeviceInterfaceData(handle, &data);
            scanner->add(dev);
        }

        // Device does not have an interface matching this GUID, skip
    }
    SetupDiDestroyDeviceInfoList(handle);
}
bool WinHotplug::nativeEventFilter(const QByteArray& eventType, void* message, long* result) {
    if (eventType == "windows_generic_MSG") {
        MSG* msg = reinterpret_cast<MSG*>(message);
        auto msgType = msg->message;
        if (msgType == WM_DEVICECHANGE) {
            PDEV_BROADCAST_HDR lpdb = reinterpret_cast<PDEV_BROADCAST_HDR>(msg->lParam);
            switch (msg->wParam) {
                case DBT_DEVICEARRIVAL:
                    if (lpdb->dbch_devicetype == DBT_DEVTYP_PORT) {
                        PDEV_BROADCAST_PORT_W lpdbv = reinterpret_cast<PDEV_BROADCAST_PORT_W>(lpdb);
                        for (auto p : QSerialPortInfo::availablePorts()) {
                            if (p.portName().contains(QString::fromWCharArray(lpdbv->dbcp_name))) {
                                m_port_list.push_back(p);
                                scanner->serialDeviceDetected(p);
                                return false;
                            }
                        }
                    } else if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                        PDEV_BROADCAST_DEVICEINTERFACE_W lpdbv = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE_W>(lpdb);
                        bool isArdwiino = lpdbv->dbcc_classguid == GUID_DEVINTERFACE_ARDWIINO;
                        UsbDevice_t dev = {};
                        lookupUSBInfo(isArdwiino, lpdbv->dbcc_name, msg->hwnd, &dev);

                        QTimer::singleShot(isArdwiino ? 1000 : 100, [this, dev]() {
                            scanner->add(dev);
                        });
                    }
                    break;
                case DBT_DEVICEREMOVECOMPLETE:
                    if (lpdb->dbch_devicetype == DBT_DEVTYP_PORT) {
                        PDEV_BROADCAST_PORT_W lpdbv = reinterpret_cast<PDEV_BROADCAST_PORT_W>(lpdb);
                        for (auto p : m_port_list) {
                            if (p.portName().contains(QString::fromWCharArray(lpdbv->dbcp_name))) {
                                m_port_list.removeOne(p);
                                scanner->serialDeviceUnplugged(p);
                                return false;
                            }
                        }
                    } else if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                        PDEV_BROADCAST_DEVICEINTERFACE_W lpdbv = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE_W>(lpdb);
                        bool isArdwiino = lpdbv->dbcc_classguid == GUID_DEVINTERFACE_ARDWIINO;
                        UsbDevice_t dev = {};
                        lookupUSBInfo(isArdwiino, lpdbv->dbcc_name, msg->hwnd, &dev);
                        scanner->remove(dev);
                    }
            }
        }
    }
    return false;
}
void WinHotplug::init(WId wid) {
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_COMPORT;

    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter2;
    ZeroMemory(&NotificationFilter2, sizeof(NotificationFilter));
    NotificationFilter2.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter2.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter2.dbcc_classguid = GUID_DEVINTERFACE_ARDWIINO;

    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter3;
    ZeroMemory(&NotificationFilter3, sizeof(NotificationFilter));
    NotificationFilter3.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter3.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter3.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;

    RegisterDeviceNotification(reinterpret_cast<HWND>(wid), &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
    RegisterDeviceNotification(reinterpret_cast<HWND>(wid), &NotificationFilter2, DEVICE_NOTIFY_WINDOW_HANDLE);
    RegisterDeviceNotification(reinterpret_cast<HWND>(wid), &NotificationFilter3, DEVICE_NOTIFY_WINDOW_HANDLE);
}
#endif
