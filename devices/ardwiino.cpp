#include "ardwiino.h"

#include <QSettings>
#include <QThread>
Ardwiino::Ardwiino(UsbDevice_t devt, QObject* parent) : Device(devt, parent), m_usbDevice(devt), m_configurable(false), m_isOpen(false) {
}
bool Ardwiino::open() {
    if (m_deviceID.serial.isEmpty()) return false;
    if (!m_usbDevice.open()) return false;
    m_isOpen = true;
    qDebug() << m_usbDevice.read(0);
    auto d = m_usbDevice.read(0);
    data_t data = *((data_t*)d.data());
    Configuration_t conf;    
    m_board = ArdwiinoLookup::findByBoard(QString::fromUtf8((char*)data.board), false);
    m_board.cpuFrequency = data.cpu_freq;
    m_extension = data.extension;
    qDebug() << m_board.name;
    // while (true) {
        //   if (data.board[0]) {
            qDebug() << sizeof(Configuration_t) << data.offset << "->" << qMin(sizeof(data.data), sizeof(Configuration_t) - data.offset);
            memcpy(((uint8_t*)&conf) + data.offset, data.data, qMin(sizeof(data.data), sizeof(Configuration_t) - data.offset));
//                if (data.offset + sizeof(data.data) >= sizeof(conf)) {
//                    break;
//                }
        /*
        data = readData();
    }*/
    m_configuration = new DeviceConfiguration(conf);
    m_configurable = !ArdwiinoLookup::isOutdatedArdwiino(m_deviceID.releaseNumber);
    // m_configurable = true;
    emit configurationChanged();
    emit configurableChanged();
    emit boardImageChanged();
    return true;
}
data_t Ardwiino::readData() {
    QByteArray data(sizeof(data_t) + 1, '\0');
    data[0] = 0;
    // hid_get_feature_report(m_hiddev, reinterpret_cast<unsigned char*>(data.data()), data.size());
    // data.remove(0, 1);
    // auto err = hid_error(m_hiddev);
    // if (err) {
    //     // TODO: handle errors (Tell the user that we could not communicate with the controller)
    //     qDebug() << QString::fromWCharArray(err);
    // }
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
    // hid_send_feature_report(m_hiddev, reinterpret_cast<unsigned char*>(data.data()), data.size());
    // auto err = hid_error(m_hiddev);
    // if (err) {
    //     // TODO: handle errors (Tell the user that we could not communicate with the controller)
    //     qDebug() << "error writing" << cmd << QString::fromWCharArray(err);
    // }
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
    // m_hiddev = NULL;
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
    return m_isOpen;
}
void Ardwiino::close() {
    if (m_isOpen) {
        m_usbDevice.close();
    }
}
void Ardwiino::bootloader() {
    writeData(COMMAND_JUMP_BOOTLOADER, {});
}
