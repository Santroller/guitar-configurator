#include "port.h"
#include "QDebug"
Port::Port(const QSerialPortInfo *serialPortInfo, QObject *parent) : QObject(parent)
{
    if (serialPortInfo) {
        m_port = serialPortInfo->portName();
        m_isArdwiino = ArdwiinoLookup::isArdwiino(serialPortInfo);
        if (m_isArdwiino) {
            m_serialPort = new QSerialPort(*serialPortInfo);
            m_description = "Ardwiino - Reading Controller Information";
            QObject::connect(m_serialPort, &QSerialPort::readyRead, this, &Port::update);
            QObject::connect(m_serialPort, &QSerialPort::errorOccurred, this, &Port::handleError);
            if (!m_serialPort->open(QIODevice::ReadWrite)) {
                m_serialPort->close();
                return;
            } else {
                m_serialPort->write("r");
                m_serialPort->waitForBytesWritten(-1);
            }
        } else {
            board = ArdwiinoLookup::detectBoard(serialPortInfo);
            qDebug() << board;
            if (board != nullptr) {
                m_description = board->name;
            }
        }
    } else {
        m_description = "Searching for devices";
        m_port = "searching";
    }
}

void Port::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError != QSerialPort::SerialPortError::NoError) {
        m_description = "Ardwiino - Error Communicating";
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

