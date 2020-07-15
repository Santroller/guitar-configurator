#include "ardwiino.h"

#include <QSettings>
#include <QThread>

#include "submodules/Ardwiino/src/shared/config/config.h"
#define USAGE_GAMEPAD 0x05
Ardwiino::Ardwiino(struct hid_device_info* usbId, UsbDevice_t devt, QObject* parent) : Device(devt, parent), m_usbId(usbId), m_configurable(false) {
    m_serialNum = QString::fromWCharArray(m_usbId->serial_number);
}
Ardwiino::Ardwiino(UsbDevice_t devt, QObject* parent) : Device(devt, parent), m_configurable(false) {
}
bool Ardwiino::open() {
#ifdef Q_OS_WIN32
    //For whatever reason the interface number is only 0 for the gamepad
    if (m_usbId->interface_number != 0) return false;
#endif
#ifdef Q_OS_MACOS
    //The gamepad usage specifically contains our feature requests, so only that one should be opened!
    if (m_usbId->usage != USAGE_GAMEPAD) return false;
#endif
    m_hiddev = hid_open_path(m_usbId->path);
    if (m_hiddev) {
        m_board = ArdwiinoLookup::findByBoard(QString::fromUtf8(readData(COMMAND_GET_BOARD)));
        m_board.cpuFrequency = QString::fromUtf8(readData(COMMAND_GET_CPU_FREQ)).trimmed().replace("UL", "").toInt();
        m_configuration = new DeviceConfiguration(*(Configuration_t*)readData(COMMAND_READ_CONFIG).data());
        m_configurable = !ArdwiinoLookup::isOutdatedArdwiino(m_usbId->release_number);
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
    writeData(id,{});
    QByteArray data(sizeof(Configuration_t), '\0');
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
void Ardwiino::writeData(int cmd, QByteArray dataToSend) {
    QByteArray data;
    data.push_back('\0');
    data.push_back(cmd);
    data.push_back(dataToSend);
    hid_send_feature_report(m_hiddev, reinterpret_cast<unsigned char*>(data.data()), data.size());
    auto err = hid_error(m_hiddev);
    if (err) {
        // TODO: handle errors (Tell the user that we could not communicate with the controller)
        qDebug() << "error writing" << cmd << QString::fromWCharArray(err);
    }
}
#define PARTIAL_CONFIG_SIZE sizeof(Configuration_t)/2
void Ardwiino::writeConfig() {
    auto config = m_configuration->getConfig();
    QByteArray data;
    data.push_back('\0');
    data.push_back(QByteArray::fromRawData(reinterpret_cast<char*>(&config), PARTIAL_CONFIG_SIZE));
    writeData(COMMAND_WRITE_CONFIG, data);
    data.clear();
    // Since we are writing big chunks to EEPROM, we need a delay before processing the next command.
    QThread::currentThread()->msleep(50);
    data.push_back(PARTIAL_CONFIG_SIZE);
    data.push_back(QByteArray::fromRawData(reinterpret_cast<char*>(&config) + PARTIAL_CONFIG_SIZE, sizeof(Configuration_t) - PARTIAL_CONFIG_SIZE));   
    writeData(COMMAND_WRITE_CONFIG, data);
    // Since we are writing big chunks to EEPROM, we need a delay before processing the next command.
    QThread::currentThread()->msleep(50);
    writeData(COMMAND_REBOOT);
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
        uint8_t ext = *readData(COMMAND_GET_EXTENSION).data();
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
    // QString cmd =
}
