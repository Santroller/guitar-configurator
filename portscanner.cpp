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

PortScanner::PortScanner(Programmer* programmer, QObject* parent) : QObject(parent), m_hasSelected(false), m_selected(nullptr), m_programmer(programmer) {
    hid_init();
    m_emptyDevice = new NullDevice();
    m_model.push_back(m_emptyDevice);
    if (settings.contains("configMode")) {
        m_graphical = settings.value("configMode").toBool();
    } else {
        m_graphical = true;
    }
    setSelected(nullptr);
}
bool PortScanner::add(Device* device) {
    if (std::find_if(m_model.begin(), m_model.end(), [device](QObject* f) { return *static_cast<Device*>(f) == *device; }) != m_model.end()) {
        return false;
    }
    if (device->open()) {
        if (!m_hasSelected && m_selected) {
            m_hasSelected = true;
            m_selected = device;
            selectedChanged();
            hasSelectedChanged();
        }
        connect(device, &Device::descriptionChanged, this, &PortScanner::update);
        m_model.append(device);
        update();
        return true;
    }
    return false;
}
bool PortScanner::remove(Device* device) {
    auto found = std::find_if(m_model.begin(), m_model.end(), [device](QObject* f) { return *static_cast<Device*>(f) == *device; });
    if (found == m_model.end()) {
        return false;
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
    return true;
}
void PortScanner::add(UsbDevice_t device) {
    DfuArduino* dev = NULL;
    if (ArdwiinoLookup::isArdwiino(device)) {
        // struct hid_device_info *devs, *cur_dev;
        // devs = hid_enumerate(device.vid, device.pid);
        // cur_dev = devs;
        // while (cur_dev) {
            // if (QString::fromWCharArray(cur_dev->serial_number) == device.serial) {
                Ardwiino* adev = new Ardwiino(device);
                if (add(adev)) {
                    m_programmer->deviceAdded(adev);
                }
            // }
            // cur_dev = cur_dev->next;
        // }
        // hid_free_enumeration(devs);
    } else if (device.vid == VID_8U2 && device.pid == PID_8U2) {
        dev = new DfuArduino("at90usb82", device);
    } else if (device.vid == VID_16U2 && device.pid == PID_16U2) {
        dev = new DfuArduino("atmega16u2", device);
    }
    if (dev && add(dev)) {
        m_programmer->deviceAdded(dev);
    }
}
void PortScanner::remove(UsbDevice_t device) {
    if (ArdwiinoLookup::isArdwiino(device)) {
        remove(new Ardwiino(device));
    } else if (device.vid == VID_8U2 && device.pid == PID_8U2) {
        remove(new DfuArduino("at90usb82", device));
    } else if (device.vid == VID_16U2 && device.pid == PID_16U2) {
        remove(new DfuArduino("atmega16u2", device));
    }
}
void PortScanner::serialDeviceDetected(const QSerialPortInfo& serialPortInfo) {
    if (ArdwiinoLookup::isArdwiino(serialPortInfo)) {
        add(new OutdatedArdwiino(serialPortInfo));
    } else {
        auto board = ArdwiinoLookup::detectBoard(serialPortInfo);
        if (board.name != "") {
            Arduino* dev = new Arduino(serialPortInfo);
            if (add(dev)) {
                m_programmer->deviceAdded(dev);
            }
        }
    }
}
void PortScanner::serialDeviceUnplugged(const QSerialPortInfo& serialPortInfo) {
    if (ArdwiinoLookup::isArdwiino(serialPortInfo)) {
        remove(new OutdatedArdwiino(serialPortInfo));
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
