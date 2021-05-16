#include "arduino.h"

#include <QThread>
Arduino::Arduino(const QSerialPortInfo &serialPortInfo, QObject *parent) : SerialDevice(serialPortInfo, parent) {
    m_board = ArdwiinoLookup::detectBoard(serialPortInfo);
}
bool Arduino::isReady() {
    return !m_board.name.isEmpty();
}
void Arduino::close() {
    SerialDevice::close();
}
bool Arduino::open() {
    return true;
}
QString Arduino::getDescription() {
    return m_board.name + " (" + m_location + ")";
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
    if (m_serialPort->open(QIODevice::WriteOnly)) {
        close();
    }
#pragma GCC diagnostic pop
}