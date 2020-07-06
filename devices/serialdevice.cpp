#include "serialdevice.h"

SerialDevice::SerialDevice(const QSerialPortInfo& serialInfo, QObject* parent) : Device({}, parent) {
    m_serialPort = new QSerialPort(serialInfo);
}
void SerialDevice::close() {
    m_serialPort->close();
}
QByteArray SerialDevice::readWrite(QByteArray data) {
    if (!m_serialPort->isOpen()) return QByteArray();
    m_serialPort->write(data);
    if (!m_serialPort->waitForBytesWritten() || !m_serialPort->waitForReadyRead()) {
        return QByteArray();
    }
    return m_serialPort->readAll();
}