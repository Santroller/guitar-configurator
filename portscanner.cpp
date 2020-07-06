#include "portscanner.h"

#include <hidapi.h>

#include <QBitmap>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QIcon>
#include <QProcess>
#include <QThread>
#include <algorithm>

#include "devices/dfu_arduino.h"
#include "devices/null_device.h"
#define PID_8U2 0x2FF7
#define VID_8U2 0x03eb
#define PID_16U2 0x2FEF
#define VID_16U2 0x03eb
static int LIBUSB_CALL hotplug_callback_c(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data) {
    return PortScanner::hotplug_callback(ctx,dev,event,user_data);
}
PortScanner::PortScanner(Programmer* programmer, QObject* parent) : QObject(parent), m_hasSelected(false), m_selected(nullptr), programmer(programmer) {
    hid_init();
    m_emptyDevice = new NullDevice();
    m_model.push_back(m_emptyDevice);
    if (settings.contains("configMode")) {
        m_graphical = settings.value("configMode").toBool();
    } else {
        m_graphical = true;
    }
    setSelected(nullptr);
    scanDevices();

    QThread* thread = QThread::create([this] {
        libusb_hotplug_callback_handle callback_handle;
        int rc;

        libusb_init(NULL);
        if (libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
            rc = libusb_hotplug_register_callback(NULL,
                                                  static_cast<libusb_hotplug_event>((LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT)),
                                                  LIBUSB_HOTPLUG_NO_FLAGS,
                                                  LIBUSB_HOTPLUG_MATCH_ANY,
                                                  LIBUSB_HOTPLUG_MATCH_ANY,
                                                  LIBUSB_HOTPLUG_MATCH_ANY,
                                                  reinterpret_cast<libusb_hotplug_callback_fn>(hotplug_callback_c),
                                                  reinterpret_cast<void *>(this),
                                                  &callback_handle);
            if (LIBUSB_SUCCESS != rc) {
                qDebug() << ("Error creating a hotplug callback\n") << rc;
                libusb_exit(NULL);
                return;
            }
            while (!QCoreApplication::instance()->closingDown()) {
                struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 0;
                libusb_handle_events_timeout_completed(NULL, &tv, NULL);
            }
        } else {
            QList<UsbDevice_t> existingDevices;
            while (!QCoreApplication::instance()->closingDown()) {
                libusb_device **devs;
                ssize_t cnt;
                cnt = libusb_get_device_list(NULL, &devs);
                QList<UsbDevice_t> devices;
                for (int i = 0; i < cnt; i++) {
                    libusb_device *dev = devs[i];
                    UsbDevice_t devt = {libusb_get_bus_number(dev),libusb_get_port_number(dev),dev};
                    if (!existingDevices.contains(devt)) {
                        hotplug_callback(NULL, dev, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, this);
                    }
                    devices << devt;
                }
                for (auto& dev: existingDevices) {
                    if (!devices.contains(dev)) {
                        hotplug_callback(NULL, dev.dev, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, this);
                    }
                }
                existingDevices.clear();
                existingDevices << devices;
                libusb_free_device_list(devs, 1);
            }

        }
    });
    thread->start();
}
void PortScanner::scanDevices() {
    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    QList<Ardwiino*> serials;
    while (cur_dev) {
        if (ArdwiinoLookup::isArdwiino(cur_dev)) {
            add(new Ardwiino(cur_dev));
            serials.push_back(new Ardwiino(cur_dev));
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    for (auto device : m_model) {
        auto ardwiino = dynamic_cast<Ardwiino*>(device);
        if (ardwiino) {
            if (std::find_if(serials.begin(), serials.end(), [ardwiino](Ardwiino* f) { return *f == *ardwiino; }) == serials.end()) {
                remove(ardwiino);
            }
        }
    }
}
int PortScanner::hotplug_callback(struct libusb_context* ctx, struct libusb_device* dev,
                                  libusb_hotplug_event event, void* user_data) {
    static libusb_device_handle* dev_handle = NULL;
    struct libusb_device_descriptor desc;
    int rc;
    PortScanner* sc = (PortScanner*)user_data;
    (void)libusb_get_device_descriptor(dev, &desc);
    // We need a small delay as we want to wait for the device to initialise
    QThread::msleep(100);
    QMetaObject::invokeMethod(sc, [sc, desc, event] {
        sc->scanDevices();
        if (desc.idVendor == VID_8U2 && desc.idProduct == PID_8U2) {
            if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
                sc->add(new DfuArduino("at90usb82"));
            } else {
                sc->remove(new DfuArduino("at90usb82"));
            }
        } else if (desc.idVendor == VID_16U2 && desc.idProduct == PID_16U2) {
            if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
                sc->add(new DfuArduino("atmega16u2"));
            } else {
                sc->remove(new DfuArduino("atmega16u2"));
            }
        }
    });
    return 0;
}
void PortScanner::add(Device* device) {
    if (std::find_if(m_model.begin(), m_model.end(), [device](QObject* f) { return *static_cast<Device*>(f) == *device; }) != m_model.end()) {
        return;
    }
    if (device->open()) {
        if (!m_hasSelected && m_selected) {
            m_hasSelected = true;
            m_selected = device;
            selectedChanged();
            hasSelectedChanged();
        }
        m_model.append(device);
        update();
    }
}
void PortScanner::remove(Device* device) {
    auto found = std::find_if(m_model.begin(), m_model.end(), [device](QObject* f) { return *static_cast<Device*>(f) == *device; });
    if (found == m_model.end()) {
        return;
    }
    auto foundEle = ((Device*)*found);
    if (m_selected == foundEle) {
        m_hasSelected = false;
        selectedChanged();
        hasSelectedChanged();
    }
    foundEle->close();
    m_model.removeAll(foundEle);
    update();
}
void PortScanner::serialDeviceDetected(const QSerialPortInfo& serialPortInfo) {
    if (ArdwiinoLookup::isArdwiino(serialPortInfo)) {
        Device* device = new OutdatedArdwiino(serialPortInfo);
        connect(device, &Device::descriptionChanged, this, &PortScanner::update);
        add(device);
    } else {
        auto board = ArdwiinoLookup::detectBoard(serialPortInfo);
        if (board.name != "") {
            add(new Arduino(serialPortInfo));
        }
    }
}
void PortScanner::serialDeviceUnplugged(const QSerialPortInfo& serialPortInfo) {
    if (ArdwiinoLookup::isArdwiino(serialPortInfo)) {
        Device* device = new OutdatedArdwiino(serialPortInfo);
        remove(device);
    } else {
        auto board = ArdwiinoLookup::detectBoard(serialPortInfo);
        if (board.name != "") {
            remove(new Arduino(serialPortInfo));
        }
    }
}
void PortScanner::update() {
    if (m_model.length() == 0) {
        m_model.push_back(m_emptyDevice);
    } else if (m_model.first() == m_emptyDevice && m_model.length() > 1) {
        m_model.removeAll(m_emptyDevice);
    }
    //For whatever reason, just updating the model is not enough to make the QML combobox update correctly. However, removing all items then adding them again does work.
    auto m = m_model;
    m_model.clear();
    m_model.push_back(m_emptyDevice);
    emit modelChanged();
    m_model.clear();
    m_model << m;
    emit modelChanged();
}
void PortScanner::setSelected(Device* port) {
    m_selected = port;
    if (port != nullptr) {
        m_hasSelected = true;
        selectedChanged();
        hasSelectedChanged();
        return;
    }
    selectedChanged();
}
// TODO: the stuff below really should be moved to a different file, it does not belong here.
void PortScanner::fixLinux() {
    QFile f("/sys/bus/usb/drivers/xpad/new_id");
    f.open(QFile::ReadOnly);
    QString s = QString(f.readAll());
    f.close();
    if (!s.contains("1209 2882")) {
        QProcess p;
        p.start("pkexec", {"tee", "-a", "/sys/bus/usb/drivers/xpad/new_id"});
        p.waitForStarted();
        p.write("1209 2882 ff");
        p.closeWriteChannel();
        p.waitForFinished();
    }
}
void PortScanner::clearImages() {
    images.clear();
}

void PortScanner::toggleGraphics() {
    m_graphical = !m_graphical;
    graphicalChanged();
    settings.setValue("configMode", m_graphical);
}
QVector<uchar> data;
int width;
int height;
QString PortScanner::findElement(QString base, int w, int h, int mouseX, int mouseY) {
    if (images.isEmpty()) {
        width = w;
        height = h;
        data.clear();
        data.resize(width * height);
        auto imageList = QDir(":/" + base + "/components").entryList();
        imageList.sort();
        QVector<QRgb> colorMap;
        colorMap.push_back(qRgba(0, 0, 0, 0));
        colorMap.push_back(qRgba(255, 255, 255, 255));
        for (auto image : imageList) {
            auto i = QIcon(":/" + base + "/components/" + image).pixmap(QSize(width, height)).toImage();
            i = i.convertToFormat(QImage::Format_Indexed8, colorMap, Qt::AutoColor);
            auto s2 = i.height() * i.width();
            auto s3 = data.data();
            auto s = i.bits();
            while (s2--) {
                if (*s != 0) {
                    *s3 = *s;
                }
                s3++;
                s++;
            }
            images.push_back(image);
            colorMap.push_front(qRgba(0, 0, 0, 0));
        }
    }
    double x = (double)mouseX / w;
    double y = (double)mouseY / h;
    if (x >= 1 || y >= 1 || x < 0 || y < 0) return "";
    x *= width;
    y *= height;
    auto c = data[(int)y * width + (int)x];
    if (c != 0) {
        return "/" + base + "/components/" + images[c - 1];
    }
    return "";
}
