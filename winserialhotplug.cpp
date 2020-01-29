#include "winserialhotplug.h"
#ifdef Q_OS_WIN

#include <QDebug>
#include <QThread>

#pragma comment(lib, "user32.lib" )

bool operator== (const QSerialPortInfo &lhs, const QSerialPortInfo &rhs)
{
  return lhs.manufacturer() == rhs.manufacturer()
    && lhs.serialNumber() == rhs.serialNumber();
}
WinSerialHotplug::WinSerialHotplug(PortScanner* scanner):scanner(scanner) {
    for (auto a:QSerialPortInfo::availablePorts()) {
        m_port_list.push_back(a);
        scanner->addPort(a);
    }
}
bool WinSerialHotplug::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "windows_generic_MSG") {
        MSG* msg = reinterpret_cast<MSG*>(message);
        auto msgType = msg->message;
        if(msgType == WM_DEVICECHANGE)
        {
            PDEV_BROADCAST_HDR lpdb = reinterpret_cast<PDEV_BROADCAST_HDR>(msg->lParam);
            switch(msg->wParam)
            {
            case DBT_DEVICEARRIVAL:
                if (lpdb -> dbch_devicetype == DBT_DEVTYP_PORT)
                {
                    PDEV_BROADCAST_PORT_W lpdbv = reinterpret_cast<PDEV_BROADCAST_PORT_W>(lpdb);
                    for (auto p: QSerialPortInfo::availablePorts()) {
                        if (p.portName().contains(QString(reinterpret_cast<QChar*>(lpdbv->dbcp_name)))) {
                            m_port_list.push_back(p);
                            QThread::msleep(500);
                            scanner->addPort(p);
                            return false;
                        }
                    }
                }
                break;
            case DBT_DEVICEREMOVECOMPLETE:
                if (lpdb -> dbch_devicetype == DBT_DEVTYP_PORT)
                {
                    PDEV_BROADCAST_PORT_W lpdbv = reinterpret_cast<PDEV_BROADCAST_PORT_W>(lpdb);
                    for (auto p: m_port_list) {
                        if (p.portName().contains(QString(reinterpret_cast<QChar*>(lpdbv->dbcp_name)))) {
                            m_port_list.removeOne(p);
                            scanner->removePort(p);
                            return false;
                        }
                    }
                }

            }
        }
    }
    return false;
}
void WinSerialHotplug::init(WId wid) {
    //On windows, it isn't enough to rely on the port list changing, so we have to use the windows API to work out if a device is hotplugged.
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_COMPORT;
    RegisterDeviceNotification(reinterpret_cast<HWND>(wid),&NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE );
}
#endif
