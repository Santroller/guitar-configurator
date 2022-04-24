#include "arduino.h"
#include <QThread>
Arduino::Arduino(const QSerialPortInfo &serialPortInfo, QObject *parent) : SerialDevice(serialPortInfo, parent) {
    m_board = ArdwiinoLookup::detectBoard(serialPortInfo);
}
bool Arduino::isReady() {
    return !m_board.name.isEmpty();
}
bool Arduino::open() {
    return true;
}
QString Arduino::getDescription() {
    return m_board.name + " (" + m_location + ")";
}
void Arduino::bootloader() {
    m_serialPort->setBaudRate(QSerialPort::Baud1200);
    m_serialPort->setDataBits(QSerialPort::DataBits::Data8);
    m_serialPort->setStopBits(QSerialPort::StopBits::OneStop);
    m_serialPort->setParity(QSerialPort::Parity::NoParity);
    m_serialPort->open(QSerialPort::ReadWrite);
    m_serialPort->setDataTerminalReady(false);
}