#include "port.h"
#include "QDebug"
#include "QThread"
Port::Port(const QSerialPortInfo &serialPortInfo, QObject *parent) : QObject(parent)
{
    rescan(serialPortInfo);
}

Port::Port(QObject *parent) : QObject(parent)
{
    m_description = "Searching for devices";
    m_port = "searching";
}

void Port::rescan(const QSerialPortInfo &serialPortInfo) {
    m_isArdwiino = ArdwiinoLookup::isArdwiino(serialPortInfo);
    if (m_isArdwiino) {
        m_description = "Ardwiino - Reading Controller Information";
        m_port = serialPortInfo.systemLocation();
    } else {
        auto b = ArdwiinoLookup::detectBoard(serialPortInfo);
        if (b != nullptr) {
            m_board = *b;
            m_port = serialPortInfo.systemLocation();
            m_description = m_board.name + " - "+m_port;
        }
    }
}

void Port::open(const QSerialPortInfo &serialPortInfo) {
    m_serialPort = new QSerialPort(serialPortInfo);
    if (m_isArdwiino) {
        QObject::connect(m_serialPort, &QSerialPort::readyRead, this, &Port::update);
        QObject::connect(m_serialPort, &QSerialPort::errorOccurred, this, &Port::handleError);
        if (m_serialPort->open(QIODevice::ReadWrite)) {
            m_serialPort->write("r");
            m_serialPort->waitForBytesWritten(-1);
        } else {
            m_serialPort->close();
        }
    }
}

void Port::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError != QSerialPort::SerialPortError::NoError && serialPortError != QSerialPort::NotOpenError) {
        m_description = "Ardwiino - Error Communicating";
        m_serialPort->close();
    }
}
bool comp(const QSerialPortInfo a, const QSerialPortInfo b)
{
    return a.portName() < b.portName();
}
void Port::prepareUpload() {
    if (m_board.protocol == "avr109") {
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
        m_serialPort->close();
        //We are jumping to the bootloader. Look for a new port that has just appeared, and assume it is the bootloader.
        findNew();
    }
}

void Port::findNew() {
    m_port_list = QSerialPortInfo::availablePorts();
    while (!findNewAsync()) {
        QThread::currentThread()->msleep(400);
    }
}
bool Port::findNewAsync() {
    auto newSp = QSerialPortInfo::availablePorts();
    std::vector<QSerialPortInfo> diff;
    std::set_difference(newSp.begin(), newSp.end(), m_port_list.begin(), m_port_list.end(), std::inserter(diff, diff.begin()), comp);
    m_port_list = newSp;
    if (diff.size() != 0) {
        auto info = diff.front();
        QThread::currentThread()->msleep(400);
        m_port = info.systemLocation();
        rescan(info);
        open(info);
        return true;
    }
    return false;
}

void Port::stopScanning() {
    if (m_isArdwiino) {
        QObject::disconnect(m_serialPort, &QSerialPort::readyRead, this, &Port::update);
        m_serialPort->close();
    }
}

void Port::update() {
    readData.append(m_serialPort->readAll());
    if (readData.length() <= static_cast<signed>(sizeof(config_t) + sizeof(controller_t))) {
        return;
    }
    m_description = "Unknown Device - " + m_port;

    config_t config;
    controller_t controller;
    memcpy(&config, readData.data()+1, sizeof(config_t));
    memcpy(&controller, readData.data()+sizeof(config_t)+1, sizeof(controller_t));
    m_description = "Ardwiino - "+ ArdwiinoLookup::lookupType(config.sub_type);
    m_description += " - " + ArdwiinoLookup::lookupExtension(config.input_type, controller.device_info);
    m_description += " - " + m_port;
    m_serialPort->flush();
    m_serialPort->write("r");
    readData.clear();
    emit descriptionChanged(m_description);
}


