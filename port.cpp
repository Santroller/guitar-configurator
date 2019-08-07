#include "port.h"
#include <QDebug>
#include <QSerialPort>
#include "submodules/Ardwiino/src/shared/input/input_wii_ext.h"
#include "submodules/Ardwiino/src/shared/config/config.h"
#include "submodules/Ardwiino/src/shared/config/defaults.h"
#include "submodules/Ardwiino/src/shared/controller/controller.h"
void appendDevice(QString &str, uint8_t type, uint16_t device) {
    switch (type) {
    case DIRECT:
        str += "Wired ";
        break;
    case WII:
        str += "Wii ";
        switch (device) {
        case NUNCHUK:
            str += "Nunchuk ";
            break;
        case CLASSIC:
            str += "Classic Controller ";
            break;
        case CLASSIC_PRO:
            str += "Classic Controller Pro ";
            break;
        case UDRAW:
            str += "UDraw Tablet ";
            break;
        case DRAWSOME:
            str += "Drawsome Tablet ";
            break;
        case GUITAR:
            str += "Guitar ";
            break;
        case DRUMS:
            str += "Drums ";
            break;
        case TURNTABLE:
            str += "Turntable ";
            break;
        case TATACON:
            str += "Taiko no Tatsujin TaTaCon (Drum controller) ";
            break;
        case MOTION_PLUS:
            str += "Motion Plus ";
            break;
        case MOTION_PLUS_NUNCHUK:
            str += "Motion Plus (Nunchuk Passthrough) ";
            break;
        case MOTION_PLUS_CLASSIC:
            str += "Motion Plus (Classic Passthrough) ";
            break;
        case NO_DEVICE:
            str += "No Device";
            break;
        default:
            str += "Unknown Device";
            break;
        }
        break;
    }
}
void appendType(QString &str, uint8_t type) {
    switch (type) {
    case XINPUT_GAMEPAD_SUBTYPE:
        str += "XInput Gamepad";
        break;
    case XINPUT_WHEEL_SUBTYPE:
        str += "XInput Wheel";
        break;
    case XINPUT_ARCADE_STICK_SUBTYPE:
        str += "XInput Arcade Stick";
        break;
    case XINPUT_FLIGHT_STICK_SUBTYPE:
        str += "XInput Flight Stick";
        break;
    case XINPUT_DANCE_PAD_SUBTYPE:
        str += "XInput Dance Pad";
        break;
    case XINPUT_GUITAR_SUBTYPE:
        str += "XInput Guitar";
        break;
    case XINPUT_GUITAR_ALTERNATE_SUBTYPE:
        str += "XInput Guitar (Alternate)";
        break;
    case XINPUT_DRUM_SUBTYPE:
        str += "XInput Drums";
        break;
    case XINPUT_GUITAR_BASS_SUBTYPE:
        str += "XInput Guitar (Bass)";
        break;
    case XINPUT_ARCADE_PAD_SUBTYPE:
        str += "XInput Arcade Pad";
        break;
    case KEYBOARD_SUBTYPE:
        str += "Keyboard";
        break;
    case SWITCH_GAMEPAD_SUBTYPE:
        str += "Switch Gamepad";
        break;
    case PS3_GAMEPAD_SUBTYPE:
        str += "PS3 Gamepad";
        break;
    case PS3_GUITAR_GH_SUBTYPE:
        str += "PS3 Guitar Hero Guitar";
        break;
    case PS3_GUITAR_RB_SUBTYPE:
        str += "PS3 Rockband Guitar";
        break;
    case PS3_DRUM_GH_SUBTYPE:
        str += "PS3 Guitar Hero Drums";
        break;
    case PS3_DRUM_RB_SUBTYPE:
        str += "PS3 Rock Band Drums";
        break;
    }
}
Port::Port(const QSerialPortInfo *serialPortInfo, QObject *parent) : QObject(parent)
{
    if (serialPortInfo) {
        m_port = serialPortInfo->portName();
        m_isArdwiino = serialPortInfo->vendorIdentifier() == 0x1209 && serialPortInfo->productIdentifier() == 0x2882;
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
    m_description = "Ardwiino - ";
    appendType(m_description, config.sub_type);
    m_description += " - ";
    appendDevice(m_description, config.input_type, controller.device_info);
    m_description += " - " + m_port;
    m_serialPort->flush();
    m_serialPort->write("r");
    readData.clear();
    emit descriptionChanged(m_description);
}

