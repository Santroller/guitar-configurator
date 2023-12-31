#include "unixhotplug.h"

#include <stdio.h>
#include <stdlib.h>

#include <QDir>
#include <QFile>
#include <QStorageInfo>
#include <QThread>
#include <QTimer>
static void getDevSerial(libusb_device* dev, uint8_t index, UsbDevice_t* devt) {
    libusb_device_handle* handle;
    char data[200];
    data[0] = '\0';
    if (libusb_open(dev, &handle) == LIBUSB_SUCCESS) {
        // Sometimes, reading the descriptor times out, but it always works the second attempt.
        while (libusb_get_string_descriptor_ascii(handle, index, reinterpret_cast<unsigned char*>(data), sizeof(data)) == LIBUSB_ERROR_TIMEOUT) {
            qDebug() << "String descriptor read timeout, retrying";
        }
        libusb_close(handle);
    } else {
        qDebug() << "Error retrieving serial";
    }
    devt->serial = QString::fromUtf8(data);
}
int c = 0;
static int LIBUSB_CALL hotplug_callback_c(libusb_context* ctx, libusb_device* dev, libusb_hotplug_event event, void* user_data) {
    (void)ctx;
    UnixHotplug* sc = (UnixHotplug*)user_data;
    libusb_ref_device(dev);
    QMetaObject::invokeMethod(sc, [sc, dev, event] {
        // We need a small delay as we want to wait for the device to initialise
        QTimer::singleShot(1000, [event, sc, dev]() {
            struct libusb_device_descriptor desc;
            (void)libusb_get_device_descriptor(dev, &desc);
            UsbDevice_t devt = {libusb_get_bus_number(dev), libusb_get_device_address(dev), desc.idVendor, desc.idProduct, desc.bcdDevice, NULL, "", "", dev};
            auto delay = 0;
            if (ArdwiinoLookup::isArdwiino(devt) && event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
                getDevSerial(dev, desc.iSerialNumber, &devt);
                delay = 900;
            }
            QTimer::singleShot(delay, [event, sc, dev, devt]() {
                sc->hotplug_callback(devt, event);
            });
            libusb_unref_device(dev);
        });
    });
    return 0;
}
UnixHotplug::UnixHotplug(PortScanner* scanner, QObject* parent) : QObject(parent), m_hasHotplug(false), scanner(scanner) {
    watcher = new QFileSystemWatcher(this);
    watcher->addPath("/dev");
    // setup signal and slot
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &UnixHotplug::deviceChanged);
    deviceChanged("");
    // // msec
    // timer->start(10);
    //     for (const auto& a : QSerialPortInfo::availablePorts()) {
    // #if defined Q_OS_MAC
    //         if (a.portName().startsWith("cu"))
    //             continue;
    // #endif
    //         m_port_list.push_back(a);
    //         scanner->serialDeviceDetected(a);
    //     }
    libusb_hotplug_callback_handle callback_handle;
    libusb_init(NULL);
    int rc;
    tick();
    m_hasHotplug = libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG);
    if (m_hasHotplug) {
        rc = libusb_hotplug_register_callback(NULL,
                                              static_cast<libusb_hotplug_event>((LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT)),
                                              LIBUSB_HOTPLUG_NO_FLAGS,
                                              LIBUSB_HOTPLUG_MATCH_ANY,
                                              LIBUSB_HOTPLUG_MATCH_ANY,
                                              LIBUSB_HOTPLUG_MATCH_ANY,
                                              reinterpret_cast<libusb_hotplug_callback_fn>(hotplug_callback_c),
                                              reinterpret_cast<void*>(this),
                                              &callback_handle);
        if (LIBUSB_SUCCESS != rc) {
            qDebug() << ("Error creating a hotplug callback\n") << rc;
            libusb_exit(NULL);
            return;
        }
        timer = new QTimer(this);
        // setup signal and slot
        connect(timer, &QTimer::timeout, this, &UnixHotplug::tick);
        // msec
        timer->start(10);

        libusb_device** devs;
        ssize_t cnt;
        cnt = libusb_get_device_list(NULL, &devs);
        for (int i = 0; i < cnt; i++) {
            libusb_device* dev = devs[i];
            struct libusb_device_descriptor desc;
            UsbDevice_t devt = {libusb_get_bus_number(dev), libusb_get_device_address(dev), 0, 0, 0, NULL, "", "", dev};

            (void)libusb_get_device_descriptor(dev, &desc);
            devt.vid = desc.idVendor;
            devt.pid = desc.idProduct;
            devt.releaseNumber = desc.bcdDevice;
            if (ArdwiinoLookup::isArdwiino(devt)) {
                getDevSerial(dev, desc.iSerialNumber, &devt);
            }
            hotplug_callback(devt, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED);
        }
        libusb_free_device_list(devs, 1);
    }
}

void UnixHotplug::tick() {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    libusb_handle_events_timeout_completed(NULL, &tv, NULL);
}
int UnixHotplug::hotplug_callback(UsbDevice_t devt, libusb_hotplug_event event) {
    QString serial;

    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
        scanner->add(devt);

    } else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        scanner->remove(devt);
    }

    return 0;
}
auto comp(const QSerialPortInfo& a, const QSerialPortInfo& b) -> bool {
    return a.portName() < b.portName();
}
void UnixHotplug::deviceChanged(const QString& path) {
    auto newSp = QSerialPortInfo::availablePorts();
    std::vector<QSerialPortInfo> diff;
    std::sort(m_port_list.begin(), m_port_list.end(), comp);
    std::sort(newSp.begin(), newSp.end(), comp);
    //Ports in new list that aren't in old (connected ports)
    std::set_difference(newSp.begin(), newSp.end(), m_port_list.begin(), m_port_list.end(), std::inserter(diff, diff.begin()), comp);
    for (const auto& p : diff) {
        if (p.vendorIdentifier() == 0) {
            //Skip ports that have not fully loaded yet.
            newSp.erase(std::remove_if(newSp.begin(), newSp.end(), [p](const QSerialPortInfo& object) { return object.systemLocation() == p.systemLocation(); }), newSp.end());
        } else {
#if defined Q_OS_MAC
            if (p.portName().startsWith("cu"))
                continue;
#endif
            scanner->serialDeviceDetected(p);
        }
    }
    diff.clear();
    //Ports in old list that aren't in new (disconnected ports)
    std::set_difference(m_port_list.begin(), m_port_list.end(), newSp.begin(), newSp.end(), std::inserter(diff, diff.begin()), comp);
    for (const auto& p : diff) {
        scanner->serialDeviceUnplugged(p);
    }
    m_port_list = newSp;

    QList<QString> newDrv;
    foreach (const QStorageInfo& storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady()) {
            QDir drive(storage.rootPath());
            QFile file(drive.filePath("INFO_UF2.txt"));
            if (file.exists()) {
                file.open(QIODevice::ReadOnly);
                if (QString(file.readAll()).toUpper().contains("RPI-RP2")) {
                    newDrv.push_back(storage.rootPath());
                }
                file.close();
            }
        }
    }
    std::vector<QString> diffDrv;
    std::sort(m_drive_list.begin(), m_drive_list.end());
    std::sort(newDrv.begin(), newDrv.end());
    //Ports in new list that aren't in old (connected ports)
    std::set_difference(newDrv.begin(), newDrv.end(), m_drive_list.begin(), m_drive_list.end(), std::inserter(diffDrv, diffDrv.begin()));
    for (const auto& drv : diffDrv) {
        scanner->picoDetected(drv);
    }
    diffDrv.clear();
    //Ports in old list that aren't in new (disconnected ports)
    std::set_difference(m_drive_list.begin(), m_drive_list.end(), newDrv.begin(), newDrv.end(), std::inserter(diffDrv, diffDrv.begin()));
    for (const auto& drv : diffDrv) {
        scanner->picoUnplugged(drv);
    }
    m_drive_list = newDrv;
}