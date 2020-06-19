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
void Arduino::bootloader() {
    close();
    m_serialPort->setBaudRate(QSerialPort::Baud1200);
    m_serialPort->setDataBits(QSerialPort::DataBits::Data8);
    m_serialPort->setStopBits(QSerialPort::StopBits::OneStop);
    m_serialPort->setParity(QSerialPort::Parity::NoParity);
    //We need this setting, but it has been deprecated. In the future when it is removed, it will be the default behaviour and will not be required anymore.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    m_serialPort->setSettingsRestoredOnClose(false);
#pragma GCC diagnostic pop
    if (m_serialPort->open(QIODevice::WriteOnly)) {
        m_serialPort->setDataTerminalReady(false);
    }
    close();
}