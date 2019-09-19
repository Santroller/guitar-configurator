#include "port.h"
#include "QDebug"
#include "QThread"
#include "input_types.h"
Port::Port(const QSerialPortInfo &serialPortInfo, QObject *parent) : QObject(parent), m_board(ArdwiinoLookup::empty)
{
    rescan(serialPortInfo);
}

Port::Port(QObject *parent) : QObject(parent), m_board(ArdwiinoLookup::empty)
{
    m_description = "Searching for devices";
    m_port = "searching";
}

void Port::close() {
    m_serialPort->close();
}
void Port::rescan(const QSerialPortInfo &serialPortInfo) {
    m_isArdwiino = ArdwiinoLookup::getInstance()->isArdwiino(serialPortInfo);
    if (m_isArdwiino) {
        m_description = "Ardwiino - Reading Controller Information";
        m_port = serialPortInfo.systemLocation();
    } else {
        auto b = ArdwiinoLookup::detectBoard(serialPortInfo);
        if (b != nullptr) {
            m_board = *b;
            m_port = serialPortInfo.systemLocation();
            m_description = m_board.name + " - "+m_port;
            boardImageChanged(getBoardImage());
        }
    }
    emit descriptionChanged(m_description);
}
void Port::read(char id, QByteArray &readData, unsigned long size) {
    readData.clear();
    m_serialPort->flush();
    m_serialPort->write(&id);
    m_serialPort->waitForBytesWritten();
    do {
        m_serialPort->waitForReadyRead();
        readData.push_back(m_serialPort->readAll());
    } while (readData.length() < static_cast<signed>(size));
}
void Port::readData() {
    QByteArray readData;
    read('f', readData, 0);
    m_board = ArdwiinoLookup::findByBoard(readData);
    m_hasDFU = ArdwiinoLookup::getInstance()->hasDFUVariant(m_board);
    boardImageChanged(getBoardImage());
    read('c', readData, sizeof(config_t));
    memcpy(&m_config, readData.data(), sizeof(config_t));
    memcpy(&m_config_device, readData.data(), sizeof(config_t));
    readDescription();
}
void Port::open(const QSerialPortInfo &serialPortInfo) {
    m_serialPort = new QSerialPort(serialPortInfo);
    if (m_isArdwiino) {
        m_serialPort->setBaudRate(57600);
        QObject::connect(m_serialPort, &QSerialPort::errorOccurred, this, &Port::handleError);
        if (m_serialPort->open(QIODevice::ReadWrite)) {
            readData();
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
        m_serialPort->close();
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

void Port::writeConfig() {
    char data[1+sizeof(config_t)] = {'w'};
    memcpy(&data[1], &m_config, sizeof(config_t));
    m_serialPort->flush();
    m_serialPort->write(data, sizeof(data));
    m_serialPort->waitForBytesWritten();
    m_serialPort->close();
    findNew();
}

void Port::readDescription() {
    controller_t controller;
    if (InputTypes::Value(m_config_device.input_type) == InputTypes::WII_TYPE) {
        QByteArray readData;
        read('r', readData, sizeof(config_t));
        memcpy(&controller, readData.data(), sizeof(controller_t));
    }
    m_description = "Ardwiino - "+ m_board.name+" - "+ArdwiinoLookup::getInstance()->lookupType(m_config_device.sub_type);
    m_description += " - " + ArdwiinoLookup::getInstance()->lookupExtension(m_config_device.input_type, controller.device_info);
    m_description += " - " + m_port;
    emit descriptionChanged(m_description);
}


