#include "ardwiino.h"

Ardwiino::Ardwiino(QUsbDevice::Id usbId, wchar_t* serialNumber, unsigned short version, QObject* parent) : Device(parent), m_usbId(usbId), m_serialNumber(serialNumber), m_version(version) {
}
void Ardwiino::open() {
    m_hiddev = hid_open(m_usbId.vid, m_usbId.pid, m_serialNumber);
    if (m_hiddev) {
        m_board = ArdwiinoLookup::findByBoard(QString::fromUtf8(readData(COMMAND_GET_BOARD)));
        m_board.cpuFrequency = QString::fromUtf8(readData(COMMAND_GET_CPU_FREQ)).trimmed().replace("UL", "").toInt();
    }
}
QByteArray Ardwiino::readData(int id) {
    QByteArray data(120, '\0');
    data[0] = id;
    hid_get_feature_report(m_hiddev, reinterpret_cast<unsigned char*>(data.data()), data.size());
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