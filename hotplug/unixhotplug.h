#ifndef UNIXSERIALHOTPLUG_H
#define UNIXSERIALHOTPLUG_H

#include <QFileSystemWatcher>
#include <QList>
#include <QObject>
#include <QSerialPortInfo>
#include <QTimer>

#include "portscanner.h"
#include "programmer.h"

extern "C" {
    #ifdef Q_OS_MACOS
    #include <libusb.h>
    #else
    #include <libusb-1.0/libusb.h>
    #endif
}

class UnixHotplug : public QObject {
    Q_OBJECT
   public:
    int hotplug_callback(UsbDevice_t devt, libusb_hotplug_event event);
    explicit UnixHotplug(PortScanner *scanner, QObject *parent = nullptr);

   signals:

   public slots:
    void tick();
    void deviceChanged(const QString &path);

   private:
    QList<UsbDevice_t> existingDevices;
    QTimer *timer;
    bool m_hasHotplug;
    QList<QSerialPortInfo> m_port_list;
    QList<QString> m_drive_list;
    PortScanner *scanner;
    QFileSystemWatcher *watcher;
};

#endif  // UNIXSERIALHOTPLUG_H
