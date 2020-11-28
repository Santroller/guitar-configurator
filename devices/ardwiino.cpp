#include "ardwiino.h"

#include <QSettings>
#include <QThread>
Ardwiino::Ardwiino(UsbDevice_t devt, QObject* parent) : Device(devt, parent), m_usbDevice(&m_deviceID), m_isOpen(false), m_configurable(false) {
}
bool Ardwiino::open() {
    if (m_deviceID.serial.isEmpty()) return false;
    if (!m_usbDevice.open()) return false;
    m_usbDevice.read(COMMAND_GET_CPU_INFO);
    m_isOpen = true;
    cpu_info_t info;
    memcpy(&info, m_usbDevice.read(COMMAND_GET_CPU_INFO).data(), sizeof(info));
    if (QString::fromUtf8(info.board).isEmpty()) {
        memcpy(&info, m_usbDevice.read(COMMAND_GET_CPU_INFO).data()+1, sizeof(info));
    }
    m_board = ArdwiinoLookup::findByBoard(QString::fromUtf8(info.board), false);
    m_board.cpuFrequency = info.cpu_freq;
    Configuration_t conf;
    uint offset = 0;
    uint offsetId = 0;
    while (offset < sizeof(Configuration_t)) {
        auto data = m_usbDevice.read(COMMAND_READ_CONFIG + offsetId);
        memcpy(((uint8_t*)&conf) + offset, data.data(), data.length());
        offset += data.length();
        offsetId++;
    }
    m_configuration = new DeviceConfiguration(conf);
    m_configurable = !ArdwiinoLookup::isOutdatedArdwiino(m_deviceID.releaseNumber);
    //TODO: This!
    // m_configurable = true;
    emit configurationChanged();
    emit configurableChanged();
    emit boardImageChanged();
    return true;
}
#define PACKET_SIZE 64
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
        m_usbDevice.write(COMMAND_WRITE_CONFIG, data);
        offset += PARTIAL_CONFIG_SIZE;
        QThread::currentThread()->msleep(100);
    }
    uint8_t st = config.main.subType;
    if (m_configuration->isDrum()) {
        st = REAL_DRUM_SUBTYPE;
    } else if (m_configuration->isGuitar()) {
        st = REAL_GUITAR_SUBTYPE;
    }
    m_usbDevice.write(COMMAND_WRITE_SUBTYPE, QByteArray(1, config.main.subType));
    QThread::currentThread()->msleep(100);
    m_usbDevice.write(COMMAND_REBOOT, {});
    // m_hiddev = NULL;
}
void Ardwiino::findDigital(QJSValue callback) {
    m_pinDetectionCallback = callback;
    m_usbDevice.write(COMMAND_FIND_DIGITAL, {});
    QTimer::singleShot(100, [&]() {
        uint8_t pin = m_usbDevice.read(COMMAND_GET_FOUND)[0];
        if (pin == 0xFF) {
            if (m_hasPinDetectionCallback) {
                findDigital(m_pinDetectionCallback);
            }
        } else {
            QJSValueList args;
            args << QJSValue(pin);
            m_pinDetectionCallback.call(args);
        }
    });
}
void Ardwiino::findAnalog(QJSValue callback) {
    m_pinDetectionCallback = callback;
    m_usbDevice.write(COMMAND_FIND_ANALOG, {});
    QTimer::singleShot(100, [&]() {
        uint8_t pin = m_usbDevice.read(COMMAND_GET_FOUND)[0];
        if (pin == 0xFF) {
            if (m_hasPinDetectionCallback) {
                findAnalog(m_pinDetectionCallback);
            }
        } else {
            QJSValueList args;
            args << QJSValue(pin);
            m_pinDetectionCallback.call(args);
        }
    });
}
void Ardwiino::startFind() {
    m_hasPinDetectionCallback = true;
}
void Ardwiino::cancelFind() {
    m_hasPinDetectionCallback = false;
}
QString Ardwiino::getDescription() {
    if (!isReady()) {
        return "Ardwiino - Unable to communicate";
    }
    if (!m_configurable) {
        return "Ardwiino - Outdated - Continue to update";
    }
    QString desc = "Ardwiino - " + m_board.name + " - " + ArdwiinoDefines::getName(m_configuration->getMainSubType());
    auto ext = m_usbDevice.read(COMMAND_GET_EXTENSION);
    m_extension = ext[0] | ext[1] >> 8;
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
void Ardwiino::resetConfig() {
    m_usbDevice.write(COMMAND_RESET, {});
    m_usbDevice.write(COMMAND_WRITE_SUBTYPE, QByteArray(1, ArdwiinoDefines::XINPUT_GUITAR_HERO_GUITAR));
    QThread::currentThread()->msleep(500);
    m_usbDevice.write(COMMAND_REBOOT, {});
}
void Ardwiino::bootloader() {
    m_usbDevice.write(COMMAND_JUMP_BOOTLOADER, {});
}
