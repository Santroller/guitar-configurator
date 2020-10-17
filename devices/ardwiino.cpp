#include "ardwiino.h"

#include <QSettings>
#include <QThread>

#define USAGE_GAMEPAD 0x05

Ardwiino::Ardwiino(UsbDevice_t devt, QObject* parent) : Device(devt, parent), m_hiddev(nullptr), m_configurable(false) {
}
bool Ardwiino::open() {
    if (m_deviceID.serial.isEmpty()) return false;
#ifdef Q_OS_LINUX
    // On linux, there is a single device that contains all interfaces.
    wchar_t ser[m_deviceID.serial.size() + 1];
    m_deviceID.serial.toWCharArray(ser);
    ser[m_deviceID.serial.size()] = '\0';
    m_hiddev = hid_open(m_deviceID.vid, m_deviceID.pid, ser);
#else
    // On mac and windows however each hid interface gets its own device, so we need to select the correct one.
    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(m_deviceID.vid, m_deviceID.pid);
    cur_dev = devs;
    while (cur_dev) {
        if (QString::fromWCharArray(cur_dev->serial_number) == m_deviceID.serial) {
#ifdef Q_OS_WIN32
            //For whatever reason the interface number is only 0 for the gamepad
            if (cur_dev->interface_number == 0) break;
#elif defined(Q_OS_MACOS)
            //The gamepad usage specifically contains our feature requests, so only that one should be opened!
            if (cur_dev->usage == USAGE_GAMEPAD) break;
#endif
        }
        cur_dev = cur_dev->next;
    }
    if (cur_dev) {
        m_hiddev = hid_open_path(cur_dev->path);
        m_deviceID.releaseNumber = cur_dev->release_number;
    }
    hid_free_enumeration(devs);
#endif
    if (m_hiddev) {
        data_t data = readData();
        Configuration_t conf;
        while (data.offset != 0 || !data.board[0]) {
            data = readData();
        }
        
        m_board = ArdwiinoLookup::findByBoard(QString::fromUtf8((char*)data.board), false);
        m_board.cpuFrequency = data.cpu_freq;
        m_extension = data.extension;
        while (true) {
            if (data.board[0]) {
                qDebug() << sizeof(Configuration_t) << data.offset << "->" << qMin(sizeof(data.data), sizeof(Configuration_t) - data.offset);
                memcpy(((uint8_t*)&conf) + data.offset, data.data, qMin(sizeof(data.data), sizeof(Configuration_t) - data.offset));
                if (data.offset + sizeof(data.data) >= sizeof(conf)) {
                    break;
                }
            }
            data = readData();
        }
        m_configuration = new DeviceConfiguration(conf);
        m_configurable = !ArdwiinoLookup::isOutdatedArdwiino(m_deviceID.releaseNumber);
        // m_configurable = true;
        emit configurationChanged();
        emit configurableChanged();
        emit boardImageChanged();
    } else {
        // TODO: handle errors (Atleast tell the user that some undetected device was found)
        qDebug() << "UNABLE TO OPEN";
    }
    return m_hiddev;
}
data_t Ardwiino::readData() {
    QByteArray data(sizeof(data_t) + 1, '\0');
    data[0] = 0;
    hid_get_feature_report(m_hiddev, reinterpret_cast<unsigned char*>(data.data()), data.size());
    data.remove(0, 1);
    auto err = hid_error(m_hiddev);
    if (err) {
        // TODO: handle errors (Tell the user that we could not communicate with the controller)
        qDebug() << QString::fromWCharArray(err);
    }
    return *(data_t*)data.data();
}
#define PACKET_SIZE 64
void Ardwiino::writeData(int cmd, QByteArray dataToSend) {
    QByteArray data(PACKET_SIZE, '\0');
    data[0] = 0;
    data[1] = cmd;
    for (int i = 0; i < dataToSend.length(); i++) {
        data[i + 2] = dataToSend[i];
    }
    hid_send_feature_report(m_hiddev, reinterpret_cast<unsigned char*>(data.data()), data.size());
    auto err = hid_error(m_hiddev);
    if (err) {
        // TODO: handle errors (Tell the user that we could not communicate with the controller)
        qDebug() << "error writing" << cmd << QString::fromWCharArray(err);
    }
}
// Reserve space for the report id, command and the offset.
#define PARTIAL_CONFIG_SIZE PACKET_SIZE - 3
void Ardwiino::writeConfig() {
    auto config = m_configuration->getConfig();
    auto configCh = reinterpret_cast<char*>(&config);
    uint offset = 0;
    QByteArray data;
    while (offset < sizeof(Configuration_t)) {
        data.clear();
        data.push_back(offset);
        data.push_back(QByteArray::fromRawData(configCh + offset, PARTIAL_CONFIG_SIZE));
        writeData(COMMAND_WRITE_CONFIG, data);
        offset += PARTIAL_CONFIG_SIZE;
        QThread::currentThread()->msleep(100);
    }
    uint8_t st = config.main.subType;
    if (m_configuration->isDrum()) {
        st = REAL_DRUM_SUBTYPE;
    } else if (m_configuration->isGuitar()) {
        st = REAL_GUITAR_SUBTYPE;
    }
    writeData(COMMAND_WRITE_SUBTYPE, QByteArray(1, config.main.subType));
    QThread::currentThread()->msleep(100);
    writeData(COMMAND_REBOOT);
    m_hiddev = NULL;
}
void Ardwiino::findDigital(QJSValue callback) {
    m_pinDetectionCallback = callback;
    QTimer::singleShot(100, [&]() {
        uint8_t pin = readData().detectedPin;
        if (pin == 0xFF) {
            findDigital(m_pinDetectionCallback);
        } else {
            QJSValueList args;
            args << QJSValue(pin);
            m_pinDetectionCallback.call(args);
            qDebug() << pin;
        }
    });
}
void Ardwiino::findAnalog(QJSValue callback) {
    m_pinDetectionCallback = callback;
    QTimer::singleShot(100, [&]() {
        uint8_t pin = readData().detectedPin;
        if (pin == 0xFF) {
            findAnalog(m_pinDetectionCallback);
        } else {
            QJSValueList args;
            args << QJSValue(pin);
            m_pinDetectionCallback.call(args);
        }
    });
}
QString Ardwiino::getDescription() {
    if (!isReady()) {
        return "Ardwiino - Unable to communicate";
    }
    QString desc = "Ardwiino - " + m_board.name + " - " + ArdwiinoDefines::getName(m_configuration->getMainSubType());
    if (m_configuration->getMainInputType() == ArdwiinoDefines::WII) {
        auto extName = ArdwiinoDefines::getName((ArdwiinoDefines::WiiExtType)m_extension);
        if (extName == "Unknown") {
            extName = "Wii Unknown Extension";
        }
        desc += " - " + extName;
    } else if (m_configuration->getMainInputType() == ArdwiinoDefines::PS2) {
        auto extName = ArdwiinoDefines::getName((ArdwiinoDefines::PsxControllerType)m_extension);
        desc += " - " + extName;
    } else {
        desc += " - " + ArdwiinoDefines::getName(m_configuration->getMainInputType());
    }
    // On windows, we can actually write the description to registry in a way that applications will pick it up.
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\System\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_1209&PID_2882", QSettings::NativeFormat);
    settings.setValue("OEMName", desc);
#endif
    return desc;
}
bool Ardwiino::isReady() {
    return m_hiddev;
}
void Ardwiino::close() {
    if (m_hiddev) {
        hid_close(m_hiddev);
        m_hiddev = NULL;
    }
}
void Ardwiino::bootloader() {
    writeData(COMMAND_JUMP_BOOTLOADER, {});
}
