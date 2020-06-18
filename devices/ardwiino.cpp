#include "ardwiino.h"
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
    }
    return m_hiddev;
}
QByteArray Ardwiino::readData(int id) {
    QByteArray data(120, '\0');
    data[0] = id;
    hid_get_feature_report(m_hiddev, reinterpret_cast<unsigned char*>(data.data()), data.size());
    qDebug() << QString::fromWCharArray(hid_error(m_hiddev));
    return data;
}
QString Ardwiino::getDescription() {
    if (!isReady()) {
        return "Ardwiino - Unable to communicate";
    }
    return "Ardwiino - " + m_board.name;
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
