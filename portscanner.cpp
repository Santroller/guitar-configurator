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
static int LIBUSB_CALL hotplug_callback_c(libusb_context* ctx, libusb_device* dev, libusb_hotplug_event event, void* user_data) {
    (void)ctx;
    PortScanner* sc = (PortScanner*)user_data;
    QMetaObject::invokeMethod(sc, [sc, dev,event] {
        struct libusb_device_descriptor desc;
        (void)libusb_get_device_descriptor(dev, &desc);
        UsbDevice_t devt = {libusb_get_bus_number(dev), libusb_get_port_number(dev), desc.idVendor, desc.idProduct};
        sc->hotplug_callback(devt, event);
    });
    return 0;
}
PortScanner::PortScanner(Programmer* programmer, QObject* parent) : QObject(parent), m_hasSelected(false), m_selected(nullptr), m_hasHotplug(false), programmer(programmer) {
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
    timer = new QTimer(this);
    // setup signal and slot
    connect(timer, &QTimer::timeout, this, &PortScanner::tick);
    // msec
    timer->start(10);
    libusb_hotplug_callback_handle callback_handle;
    libusb_init(&ctx_scan);
    int rc;
    libusb_init(&ctx);
    tick();
    m_hasHotplug = libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG);
    if (m_hasHotplug) {
        rc = libusb_hotplug_register_callback(ctx_scan,
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
            libusb_exit(ctx);
            return;
        }
    }
}
typedef CRITICAL_SECTION usbi_mutex_t;
struct list_head {
    struct list_head *prev, *next;
};
#define PTR_ALIGN(v) \
    (((v) + (sizeof(void *) - 1)) & ~(sizeof(void *) - 1))
struct libusb_device {
    /* lock protects refcnt, everything else is finalized at initialization
     * time */
    usbi_mutex_t lock;
    int refcnt;

    struct libusb_context *ctx;

    uint8_t bus_number;
    uint8_t port_number;
    struct libusb_device* parent_dev;
    uint8_t device_address;
    uint8_t num_configurations;
    enum libusb_speed speed;

    struct list_head list;
    unsigned long session_data;

    struct libusb_device_descriptor device_descriptor;
    int attached;
};
struct winusb_device_priv {
    bool initialized;
    bool root_hub;
    uint8_t active_config;
    uint8_t depth; // distance to HCD
    const struct windows_usb_api_backend *apib;
    char *dev_id;
    char *path;  // device interface path
    int sub_api; // for WinUSB-like APIs
    struct {
        char *path; // each interface needs a device interface path,
        const struct windows_usb_api_backend *apib; // an API backend (multiple drivers support),
        int sub_api;
        int8_t nb_endpoints; // and a set of endpoint addresses (USB_MAXENDPOINTS)
        uint8_t *endpoint;
        int current_altsetting;
        bool restricted_functionality;  // indicates if the interface functionality is restricted
                        // by Windows (eg. HID keyboards or mice cannot do R/W)
    } usb_interface[32];
    struct hid_device_priv *hid;
};
struct libusb_winusb_path {
    uint8_t unused[100];
    char* path;
};

static inline void *usbi_get_device_priv(struct libusb_device *dev)
{
    return (unsigned char *)dev + PTR_ALIGN(sizeof(*dev));
}
void PortScanner::tick() {
    if (m_hasHotplug) {
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        libusb_handle_events_timeout_completed(ctx_scan, &tv, NULL);
    } else {
        libusb_device** devs;
        ssize_t cnt;
        cnt = libusb_get_device_list(ctx_scan, &devs);
        QList<UsbDevice_t> devices;
        for (int i = 0; i < cnt; i++) {
            libusb_device* dev = devs[i];
            struct libusb_device_descriptor desc;
            UsbDevice_t devt = {libusb_get_bus_number(dev), libusb_get_port_number(dev), 0, 0};

            if (!existingDevices.contains(devt)) {
                (void)libusb_get_device_descriptor(dev, &desc);
                devt.vid = desc.idVendor;
                devt.pid = desc.idProduct;
#ifdef Q_OS_WIN
                struct winusb_device_priv* priv = (winusb_device_priv*)usbi_get_device_priv(dev);
                uint8_t* devAddr = (uint8_t*)dev;
                uint8_t* pathAddr = (uint8_t*)&priv->usb_interface[0].path;
                qDebug() <<  QString::fromUtf8(((libusb_winusb_path*)dev)->path).split("\\")[2];
                devt.serial = QString::fromUtf8(priv->usb_interface[0].path).split("#")[2];
#else
                libusb_device_handle* handle;
                char data[200];
                libusb_open(dev, &handle);
                libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, reinterpret_cast<unsigned char*>(data), sizeof(data));
                libusb_close(handle);
                devt.serial = QString::fromUtf8(data);
#endif
//                hotplug_callback(devt, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED);
            }
            devices << devt;
        }
        for (auto& dev : existingDevices) {
            if (!devices.contains(dev)) {
                hotplug_callback(dev, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT);
                existingDevices.removeAll(dev);
            }
        }
        existingDevices.clear();
        existingDevices << devices;
        libusb_free_device_list(devs, 1);
    }
}
void PortScanner::scanDevices() {
}
int PortScanner::hotplug_callback(UsbDevice_t devt, libusb_hotplug_event event) {
    QString serial;
    // We need a small delay as we want to wait for the device to initialise
    QTimer::singleShot(100, [event, this, devt, serial]() {
        if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
            if (ArdwiinoLookup::isArdwiino(devt)) {
                struct hid_device_info *devs, *cur_dev;
                devs = hid_enumerate(devt.vid, devt.pid);
                cur_dev = devs;
                while (cur_dev) {
                    qDebug() << cur_dev->path;
                    add(new Ardwiino(cur_dev, devt));
                    cur_dev = cur_dev->next;
                }
                hid_free_enumeration(devs);
            } else  if (devt.vid == VID_8U2 && devt.pid == PID_8U2) {
                    add(new DfuArduino("at90usb82", devt));
            } else if (devt.vid == VID_16U2 && devt.pid == PID_16U2) {
                    add(new DfuArduino("atmega16u2", devt));
            }
        } else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
            remove(new Ardwiino(devt));
            remove(new DfuArduino(devt));
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
