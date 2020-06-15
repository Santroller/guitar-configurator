#include "arduino.h"

#include <QThread>
#define OLD_ARDWIINO_API_SERIAL "1.2"
Arduino::Arduino(const QSerialPortInfo &serialPortInfo, QObject *parent) : SerialDevice(serialPortInfo, parent) {
    m_board = ArdwiinoLookup::detectBoard(serialPortInfo);
}
bool Arduino::isReady() {
    return !m_board.name.isEmpty() && m_serialPort->isOpen();
}
void Arduino::close() {
    SerialDevice::close();
}
bool Arduino::open() {
    return true;
}
QString Arduino::getDescription() {
    return m_board.name;
}