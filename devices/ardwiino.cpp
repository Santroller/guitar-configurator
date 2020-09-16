#include "ardwiino.h"

#include <QSettings>
#include <QThread>

#include "submodules/Ardwiino/src/shared/config/config.h"
#define USAGE_GAMEPAD 0x05

Ardwiino::Ardwiino(UsbDevice_t devt, QObject* parent) : Device(devt, parent), m_configurable(false) {
}
bool Ardwiino::open() {
    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(m_deviceID.vid, m_deviceID.pid);
    cur_dev = devs;
    while (cur_dev) {
        if (QString::fromWCharArray(cur_dev->serial_number) == m_deviceID.serial) {
#ifdef Q_OS_WIN32
            //For whatever reason the interface number is only 0 for the gamepad
            if (cur_dev->interface_number == 0) break;
#elif Q_OS_MACOS
            //The gamepad usage specifically contains our feature requests, so only that one should be opened!
            if (cur_dev->usage != USAGE_GAMEPAD) break;
#else
            break;
#endif
        }
        cur_dev = cur_dev->next;
    }
    if (cur_dev) {
        m_hiddev = hid_open_path(cur_dev->path);
        m_deviceID.releaseNumber = cur_dev->release_number;
    }
    hid_free_enumeration(devs);
    if (m_hiddev) {
        m_board = ArdwiinoLookup::findByBoard(QString::fromUtf8(readData(COMMAND_GET_BOARD)), false);
        m_board.cpuFrequency = QString::fromUtf8(readData(COMMAND_GET_CPU_FREQ)).trimmed().replace("UL", "").toInt();
        m_configuration = new DeviceConfiguration(*(Configuration_t*)readData(COMMAND_READ_CONFIG).data());
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
QByteArray Ardwiino::readData(int id) {
    writeData(id, {});
    QThread::currentThread()->msleep(100);
    writeData(id, {});
    QThread::currentThread()->msleep(100);
    QByteArray data(sizeof(Configuration_t) + 1, '\0');
    data[0] = 0;
    hid_get_feature_report(m_hiddev, reinterpret_cast<unsigned char*>(data.data()), data.size());
    data.remove(0, 1);
    auto err = hid_error(m_hiddev);
    if (err) {
        // TODO: handle errors (Tell the user that we could not communicate with the controller)
        qDebug() << QString::fromWCharArray(err);
    }
    return data;
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
    writeData(COMMAND_WRITE_SUBTYPE, QByteArray(1, config.main.subType));
    QThread::currentThread()->msleep(100);
    writeData(COMMAND_REBOOT);
}
void Ardwiino::findDigital(QJSValue callback) {
    m_pinDetectionCallback = callback;
    QTimer::singleShot(100, [&]() {
        uint8_t pin = readData(COMMAND_FIND_DIGITAL).data()[0];
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
        uint8_t pin = readData(COMMAND_FIND_ANALOG).data()[0];
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
        uint16_t ext = *(uint16_t*)readData(COMMAND_GET_EXTENSION).data();
        auto extName = ArdwiinoDefines::getName((ArdwiinoDefines::WiiExtType)ext);
        if (extName == "Unknown") {
            extName = "Wii Unknown Extension";
        }
        desc += " - " + extName;
    } else if (m_configuration->getMainInputType() == ArdwiinoDefines::PS2) {
        uint8_t ext = readData(COMMAND_GET_EXTENSION).data()[0];
        qDebug() << ext;
        auto extName = ArdwiinoDefines::getName((ArdwiinoDefines::PsxControllerType)ext);
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
