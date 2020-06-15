#include "ardwiino.h"

Ardwiino::Ardwiino(struct hid_device_info* usbId, QObject* parent) : Device(parent), m_usbId(usbId) {
    m_serialNum = QString::fromWCharArray(m_usbId->serial_number);
}
bool Ardwiino::open() {
    m_hiddev = hid_open(m_usbId->vendor_id, m_usbId->product_id, m_usbId->serial_number);
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