#include "portscanner.h"

#include <hidapi/hidapi.h>

#include <QBitmap>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QIcon>
#include <QProcess>
#include <algorithm>
#if Q_OS_UNIX
#include <libusb-1.0/libusb.h>
#else
#include <libusb/libusb.h>
#endif

#include "devices/dfu_arduino.h"
#include "devices/null_device.h"
#define PID_8U2 0x2FF7
#define VID_8U2 0x03eb
#define PID_16U2 0x2FEF
#define VID_16U2 0x03eb
PortScanner::PortScanner(Programmer* programmer, QObject* parent) : QObject(parent), m_hasSelected(false), m_selected(nullptr), programmer(programmer) {
    m_emptyDevice = new NullDevice();
    m_model.push_back(m_emptyDevice);
    if (settings.contains("configMode")) {
        m_graphical = settings.value("configMode").toBool();
    } else {
        m_graphical = true;
    }
    setSelected(nullptr);
    connect(&usbInfo, &QUsbInfo::deviceInserted, this, &PortScanner::usbDeviceDetected);
    connect(&usbInfo, &QUsbInfo::deviceRemoved, this, &PortScanner::usbDeviceUnplugged);
    for (auto dev : usbInfo.devices()) {
        usbDeviceDetected(dev);
    }
}
void PortScanner::usbDeviceDetected(const QUsbDevice::Id& id) {
    if (id.vid == VID_8U2 && id.pid == PID_8U2) {
        add(new DfuArduino("at90usb82", id));
    } else if (id.vid == VID_16U2 && id.pid == VID_16U2) {
        add(new DfuArduino("atmega16u2", id));
    } else if (ArdwiinoLookup::isArdwiino(id)) {
        struct hid_device_info *devs, *cur_dev;
        devs = hid_enumerate(0x0, 0x0);
        cur_dev = devs;
        while (cur_dev) {
            if (cur_dev->vendor_id == id.vid && cur_dev->product_id == id.pid) {
                add(new Ardwiino(id, cur_dev->serial_number, cur_dev->release_number));
            }
            cur_dev = cur_dev->next;
        }
        hid_free_enumeration(devs);
    }
}
void PortScanner::usbDeviceUnplugged(const QUsbDevice::Id& id) {
    if (id.vid == VID_8U2 && id.pid == PID_8U2) {
        remove(new DfuArduino("at90usb82", id));
    } else if (id.vid == VID_16U2 && id.pid == VID_16U2) {
        remove(new DfuArduino("atmega16u2", id));
    } else if (ArdwiinoLookup::isArdwiino(id)) {
        struct hid_device_info *devs, *cur_dev;
        devs = hid_enumerate(0x0, 0x0);
        cur_dev = devs;
        QList<Ardwiino*> serials;
        while (cur_dev) {
            if (cur_dev->vendor_id == id.vid && cur_dev->product_id == id.pid) {
               serials.push_back(new Ardwiino(id, cur_dev->serial_number, cur_dev->release_number));
            }
            cur_dev = cur_dev->next;
        }
        hid_free_enumeration(devs);
        for (auto device: m_model) {
            auto ardwiino = dynamic_cast<Ardwiino*>(device);
            if (ardwiino) {
                if (std::find_if(serials.begin(), serials.end(), [ardwiino](Ardwiino* f) { return *f == *ardwiino; }) == serials.end()) {
                    remove(ardwiino);
                }
            }
        }
    }
}
void PortScanner::add(Device* device) {
    if (std::find_if(m_model.begin(), m_model.end(), [device](QObject* f) { return *static_cast<Device*>(f) == *device; }) != m_model.end()) {
        return;
    }
    m_model.append(device);
    device->open();
    update();
}
void PortScanner::remove(Device* device) {
    auto found = std::find_if(m_model.begin(), m_model.end(), [device](QObject* f) { return *static_cast<Device*>(f) == *device; });
    if (found == m_model.end()) {
        return;
    }
    auto foundEle = *found;
    m_model.removeAll(foundEle);
    ((Device*)foundEle)->close();
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
    // if (programmer->getStatus() == Status::AVRDUDE) return;
    // if (m_selected != nullptr && m_hasSelected) {
    //     m_selected->handleConnection(serialPortInfo);
    //     programmer->program(m_selected);
    //     if (!programmer->getRestore()) {
    //         m_selected->handleConnection(serialPortInfo);
    //         return;
    //     }
    // }
    // auto port = new Port(serialPortInfo);
    // if (port->getPort() == nullptr) return;
    // m_model.erase(std::remove_if(m_model.begin(), m_model.end(), [](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == "searching";}),m_model.end());
    // m_model.push_back(port);
    // port->open(serialPortInfo);
    // connect(port, &Port::typeChanged, this, &PortScanner::clearImages);
}
void PortScanner::serialDeviceUnplugged(const QSerialPortInfo& serialPortInfo) {
    // if (m_selected != nullptr) {
    //     m_selected->handleDisconnection(serialPortInfo);
    // }
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
// void PortScanner::complete(int exitCode, QProcess::ExitStatus exitStatus) {
//     (void)exitCode;
//     (void)exitStatus;
//     auto dir = QDir(QCoreApplication::applicationDirPath());
//     dir.cd("binaries");
//     // int i = 0;
//     if (m_selected != nullptr) return;
//     // for (auto process: m_process) {
//     //     if (process != nullptr) {
//     //         if (process->exitCode() == exitCode && process->state() == process->NotRunning) {
//     //             QObject::disconnect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &PortScanner::complete);
//     //             QObject::disconnect(qApp, SIGNAL(aboutToQuit()), process, SLOT(terminate()));
//     //             auto board = ArdwiinoLookup::boards[i];
//     //             if (exitCode == 0) {
//     //                 if(std::find_if(m_model.begin(), m_model.end(), [board](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == "dfu" && (dynamic_cast<Port*>(object))->getBoard().processor == board.processor;}) == m_model.end()) {
//     //                     m_model.push_back(new Port(board));
//     //                     m_model.erase(std::remove_if(m_model.begin(), m_model.end(), [](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == "searching";}),m_model.end());
//     //                     emit modelChanged();
//     //                 }
//     //             } else {
//     //                 m_model.erase(std::remove_if(m_model.begin(), m_model.end(), [board](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == "dfu" && (dynamic_cast<Port*>(object))->getBoard().processor == board.processor;}),m_model.end());
//     //             }
//     //             m_process[i] = new QProcess();
//     //             m_process[i]->setWorkingDirectory(dir.path());
//     //             connect(qApp, SIGNAL(aboutToQuit()), m_process[i], SLOT(terminate()));
//     //             connect(m_process[i], static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &PortScanner::complete);
//     //             m_process[i]->start(dir.filePath("dfu-programmer"), {ArdwiinoLookup::boards[i].processor, "get"});
//     //         }
//     //     }
//     //     i++;
//     // }
// }
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
