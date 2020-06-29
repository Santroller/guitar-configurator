#include "ardwiino.h"
#include <QSettings>
#include "submodules/Ardwiino/src/shared/config/config.h"
#define USAGE_GAMEPAD 0x05
Ardwiino::Ardwiino(struct hid_device_info* usbId, QObject* parent) : Device(parent), m_usbId(usbId) {
    m_serialNum = QString::fromWCharArray(m_usbId->serial_number);
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
        configuration = *(Configuration_t*)readData(COMMAND_READ_CONFIG).data();
    } else {
        qDebug() << "UNABLE TO OPEN";
    }
    return m_hiddev;
}
QByteArray Ardwiino::readData(int id) {
    QByteArray data(sizeof(Configuration_t), '\0');
    data[0] = 0;
    data[1] = id;
    hid_send_feature_report(m_hiddev, reinterpret_cast<unsigned char*>(data.data()), data.size());
    data[0] = 0;
    hid_get_feature_report(m_hiddev, reinterpret_cast<unsigned char*>(data.data()), data.size());
    data.remove(0,1);
    auto err = hid_error(m_hiddev);
    if (err) {
        // TODO: handle errors
        qDebug() << QString::fromWCharArray(err);
    }
    return data;
}
QString Ardwiino::getDescription() {
    if (!isReady()) {
        return "Ardwiino - Unable to communicate";
    }
    QString desc = "Ardwiino - " + m_board.name + " - " + ArdwiinoDefines::getName((ArdwiinoDefines::SubType)configuration.main.subType);
    if (configuration.main.inputType == ArdwiinoDefines::WII) {
        uint16_t ext = *(uint16_t*)readData(COMMAND_GET_EXTENSION).data();
        auto extName = ArdwiinoDefines::getName((ArdwiinoDefines::WiiExtType)ext);
        if (extName == "Unknown") {
            extName = "Wii Unknown Extension";
        }
        desc += " - " + extName;
    } else if (configuration.main.inputType == ArdwiinoDefines::WII) {
        uint8_t ext = *readData(COMMAND_GET_EXTENSION).data();
        auto extName = ArdwiinoDefines::getName((ArdwiinoDefines::PsxControllerType)ext);
        desc += " - " + extName;
    } else {
        desc += " - " + ArdwiinoDefines::getName((ArdwiinoDefines::InputType)configuration.main.inputType);
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