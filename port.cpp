#include "port.h"
#include "QDebug"
#include "QThread"
#include <QProcess>
#include "input_types.h"
#include <iostream>
#include <QSettings>
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
    if (m_serialPort != nullptr) {
        m_serialPort->close();
        portStateChanged(getOpen());
    }
}

void Port::handleConnection(const QSerialPortInfo& info) {
    if (m_serialPort && !m_serialPort->isOpen()) {
        rescan(info);
        open(info);
    }
}

void Port::rescan(const QSerialPortInfo &serialPortInfo) {
    m_isArdwiino = ArdwiinoLookup::getInstance()->isArdwiino(serialPortInfo);
    if (m_isArdwiino) {
        m_description = "Ardwiino - Reading Controller Information";
        m_port = serialPortInfo.systemLocation();
    } else {
        auto b = ArdwiinoLookup::detectBoard(serialPortInfo);
        if (!b.name.isEmpty()) {
            m_board = b;
            m_port = serialPortInfo.systemLocation();
            m_description = m_board.name + " - "+m_port;
            boardImageChanged(getBoardImage());
        }
    }
    emit descriptionChanged(m_description);
}
char Port::read_single(QByteArray id) {
    return read(id).data()[0];
}
QByteArray Port::read(QByteArray id) {
    m_serialPort->flush();
    m_serialPort->write(id);
    m_serialPort->waitForBytesWritten();
    m_serialPort->waitForReadyRead();
    return m_serialPort->readLine();
}
void Port::readData() {
    m_board = ArdwiinoLookup::findByBoard(read(READ_INFO(INFO_BOARD)).trimmed());
    m_hasDFU = m_board.hasDFU;
    boardImageChanged(getBoardImage());
    readDescription();
}
void Port::write(char id, void* dest, unsigned long size) {
    auto read = m_serialPort->readAll();
    m_serialPort->flush();
    m_serialPort->write(&id, 1);
    m_serialPort->waitForBytesWritten();
    m_serialPort->waitForReadyRead();
    if (read != "READY") {
        qDebug() << "Not ready!!!!!";
        qDebug() << read;
        return;
    }
    m_serialPort->write(static_cast<char*>(dest), static_cast<signed long>(size));
    m_serialPort->waitForBytesWritten();
    m_serialPort->waitForReadyRead();
    read = m_serialPort->readAll();
    if (read != "OK") {
        qDebug() << "Not okay!!!!!";
        qDebug() << read;
    }
}
void Port::writeConfig() {
//    write(MAIN_CMD_W, &m_config.main, sizeof(main_config_t));
//    write(PIN_CMD_W, &m_config.pins, sizeof(pins_t));
//    write(AXIS_CMD_W, &m_config.axis, sizeof(axis_config_t));
//    write(KEY_CMD_W, &m_config.keys, sizeof(keys_t));
//    char data = REBOOT_CMD;
//    m_serialPort->flush();
//    m_serialPort->write(&data, 1);
//    m_serialPort->waitForBytesWritten();
//    m_serialPort->close();
//    portStateChanged(getOpen());
}

void Port::readDescription() {
    QByteArray readData;
    auto vtype = InputTypes::Value(read_single(READ_CONFIG(CONFIG_INPUT_TYPE)));
    auto ctype = Controllers::Value(read_single(READ_CONFIG(CONFIG_SUB_TYPE)));
    m_description = "Ardwiino - "+ m_board.name+" - "+Controllers::toString(ctype);
    m_description += " - " + InputTypes::toString(vtype);
    if (vtype == InputTypes::WII_TYPE) {
        m_description += " " + read(READ_INFO(INFO_WII_EXT)).trimmed();
    }
    m_description += " - " + m_port;
    updateControllerName();
    emit descriptionChanged(m_description);
}

void Port::open(const QSerialPortInfo &serialPortInfo) {
    m_serialPort = new QSerialPort(serialPortInfo);
    if (m_isArdwiino) {
        m_serialPort->setBaudRate(QSerialPort::Baud115200);
        m_serialPort->setDataBits(QSerialPort::DataBits::Data8);
        m_serialPort->setStopBits(QSerialPort::StopBits::OneStop);
        m_serialPort->setParity(QSerialPort::Parity::NoParity);
        QObject::connect(m_serialPort, &QSerialPort::errorOccurred, this, &Port::handleError);
        if (m_serialPort->open(QIODevice::ReadWrite)) {
            QThread *thread = QThread::create([this]{
                //We need a delay on linux for unos, as the 328p gets rebooted when we open.
                QThread::msleep(2000);
                QMetaObject::invokeMethod(this, [this] { readData(); });
            });
            thread->start();
        }
    }
    portStateChanged(getOpen());
}

void Port::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError != QSerialPort::SerialPortError::NoError && serialPortError != QSerialPort::NotOpenError) {
        m_description = "Ardwiino - Error Communicating";
        if (m_serialPort->isOpen()) {
            m_serialPort->close();
            portStateChanged(getOpen());
        }
    }
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
    } else if (m_board.protocol == "arduino") {
        if (!m_serialPort->isOpen()) {
            m_serialPort->open(QIODevice::WriteOnly);
        }
//        char data[] = {BOOTLOADER_CMD};
//        m_serialPort->write(data, 1);
//        m_serialPort->waitForBytesWritten();
//        m_serialPort->close();
    }
    portStateChanged(getOpen());
}

void Port::prepareRescan() {
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
    portStateChanged(getOpen());
}

void Port::updateControllerName() {
    //On windows, update the controller name, so that users see the current device name in games
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\System\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_1209&PID_2882", QSettings::NativeFormat);
    settings.setValue("OEMName", m_description);
#endif
}

void Port::loadKeys() {
    m_pins.clear();
//    m_pins["up"] = m_config.keys.up;
//    m_pins["down"] = m_config.keys.down;
//    m_pins["left"] = m_config.keys.left;
//    m_pins["right"] = m_config.keys.right;
//    m_pins["start"] = m_config.keys.start;
//    m_pins["back"] = m_config.keys.back;
//    m_pins["left_stick"] = m_config.keys.left_stick;
//    m_pins["right_stick"] = m_config.keys.right_stick;
//    m_pins["LB"] = m_config.keys.LB;
//    m_pins["RB"] = m_config.keys.RB;
//    m_pins["home"] = m_config.keys.home;
//    m_pins["capture"] = m_config.keys.capture;
//    m_pins["a"] = m_config.keys.a;
//    m_pins["b"] = m_config.keys.b;
//    m_pins["x"] = m_config.keys.x;
//    m_pins["y"] = m_config.keys.y;
//    m_pins["lt"] = m_config.keys.lt;
//    m_pins["rt"] = m_config.keys.rt;
//    m_pins["l_x_lt"] = m_config.keys.l_x.neg;
//    m_pins["l_x_gt"] = m_config.keys.l_x.pos;
//    m_pins["l_y_lt"] = m_config.keys.l_y.neg;
//    m_pins["l_y_gt"] = m_config.keys.l_y.pos;
//    m_pins["r_x_lt"] = m_config.keys.r_x.neg;
//    m_pins["r_x_gt"] = m_config.keys.r_x.pos;
//    m_pins["r_y_lt"] = m_config.keys.r_y.neg;
//    m_pins["r_y_gt"] = m_config.keys.r_y.pos;
}

void Port::saveKeys() {
//    m_config.keys.up = uint8_t(m_pins["up"].toUInt());
//    m_config.keys.down = uint8_t(m_pins["down"].toUInt());
//    m_config.keys.left = uint8_t(m_pins["left"].toUInt());
//    m_config.keys.right = uint8_t(m_pins["right"].toUInt());
//    m_config.keys.start = uint8_t(m_pins["start"].toUInt());
//    m_config.keys.back = uint8_t(m_pins["back"].toUInt());
//    m_config.keys.left_stick = uint8_t(m_pins["left_stick"].toUInt());
//    m_config.keys.right_stick = uint8_t(m_pins["right_stick"].toUInt());
//    m_config.keys.LB = uint8_t(m_pins["LB"].toUInt());

//    m_config.keys.RB = uint8_t(m_pins["RB"].toUInt());
//    m_config.keys.home = uint8_t(m_pins["home"].toUInt());
//    m_config.keys.capture = uint8_t(m_pins["capture"].toUInt());
//    m_config.keys.a = uint8_t(m_pins["a"].toUInt());
//    m_config.keys.b = uint8_t(m_pins["b"].toUInt());
//    m_config.keys.x = uint8_t(m_pins["x"].toUInt());
//    m_config.keys.y = uint8_t(m_pins["y"].toUInt());
//    m_config.keys.lt = uint8_t(m_pins["lt"].toUInt());
//    m_config.keys.rt = uint8_t(m_pins["rt"].toUInt());
//    m_config.keys.l_x.neg = uint8_t(m_pins["l_x_lt"].toUInt());
//    m_config.keys.l_x.pos = uint8_t(m_pins["l_x_gt"].toUInt());
//    m_config.keys.l_y.neg = uint8_t(m_pins["l_y_lt"].toUInt());
//    m_config.keys.l_y.pos = uint8_t(m_pins["l_y_gt"].toUInt());
//    m_config.keys.r_x.neg = uint8_t(m_pins["r_x_lt"].toUInt());
//    m_config.keys.r_x.pos = uint8_t(m_pins["r_x_gt"].toUInt());
//    m_config.keys.r_y.neg = uint8_t(m_pins["r_y_lt"].toUInt());
//    m_config.keys.r_y.pos = uint8_t(m_pins["r_y_gt"].toUInt());
}

void Port::loadPins() {
//    m_pins.clear();
//    m_pins["up"] = m_config.pins.up;
//    m_pins["down"] = m_config.pins.down;
//    m_pins["left"] = m_config.pins.left;
//    m_pins["right"] = m_config.pins.right;
//    m_pins["start"] = m_config.pins.start;
//    m_pins["back"] = m_config.pins.back;
//    m_pins["left_stick"] = m_config.pins.left_stick;
//    m_pins["right_stick"] = m_config.pins.right_stick;
//    m_pins["LB"] = m_config.pins.LB;
//    m_pins["RB"] = m_config.pins.RB;
//    m_pins["home"] = m_config.pins.home;
//    m_pins["unused"] = m_config.pins.unused;
//    m_pins["a"] = m_config.pins.a;
//    m_pins["b"] = m_config.pins.b;
//    m_pins["x"] = m_config.pins.x;
//    m_pins["y"] = m_config.pins.y;
//    m_pins["lt"] = m_config.pins.lt;
//    m_pins["rt"] = m_config.pins.rt;
//    m_pins["l_x"] = m_config.pins.l_x;
//    m_pins["l_y"] = m_config.pins.l_y;
//    m_pins["r_x"] = m_config.pins.r_x;
//    m_pins["r_y"] = m_config.pins.r_y;
//    m_pin_inverts["lt"] = m_config.axis.inversions.lt;
//    m_pin_inverts["rt"] = m_config.axis.inversions.rt;
//    m_pin_inverts["l_x"] = m_config.axis.inversions.l_x;
//    m_pin_inverts["l_y"] = m_config.axis.inversions.l_y;
//    m_pin_inverts["r_x"] = m_config.axis.inversions.r_x;
//    m_pin_inverts["r_y"] = m_config.axis.inversions.r_y;
}

void Port::savePins() {
//    qDebug() << m_pins;
//    m_config.pins.up = uint8_t(m_pins["up"].toUInt());
//    m_config.pins.down = uint8_t(m_pins["down"].toUInt());
//    m_config.pins.left = uint8_t(m_pins["left"].toUInt());
//    m_config.pins.right = uint8_t(m_pins["right"].toUInt());
//    m_config.pins.start = uint8_t(m_pins["start"].toUInt());
//    m_config.pins.back = uint8_t(m_pins["back"].toUInt());
//    m_config.pins.left_stick = uint8_t(m_pins["left_stick"].toUInt());
//    m_config.pins.right_stick = uint8_t(m_pins["right_stick"].toUInt());
//    m_config.pins.LB = uint8_t(m_pins["LB"].toUInt());
//    m_config.pins.RB = uint8_t(m_pins["RB"].toUInt());
//    m_config.pins.home = uint8_t(m_pins["home"].toUInt());
//    m_config.pins.unused = uint8_t(m_pins["unused"].toUInt());
//    m_config.pins.a = uint8_t(m_pins["a"].toUInt());
//    m_config.pins.b = uint8_t(m_pins["b"].toUInt());
//    m_config.pins.x = uint8_t(m_pins["x"].toUInt());
//    m_config.pins.y = uint8_t(m_pins["y"].toUInt());
//    m_config.pins.lt = uint8_t(m_pins["lt"].toUInt());
//    m_config.pins.rt = uint8_t(m_pins["rt"].toUInt());
//    m_config.pins.l_x = uint8_t(m_pins["l_x"].toUInt());
//    m_config.pins.l_y = uint8_t(m_pins["l_y"].toUInt());
//    m_config.pins.r_x = uint8_t(m_pins["r_x"].toUInt());
//    m_config.pins.r_y = uint8_t(m_pins["r_y"].toUInt());
//    m_config.axis.inversions.lt = m_pin_inverts["lt"].toBool();
//    m_config.axis.inversions.rt = m_pin_inverts["rt"].toBool();
//    m_config.axis.inversions.l_x = m_pin_inverts["l_x"].toBool();
//    m_config.axis.inversions.l_y = m_pin_inverts["l_y"].toBool();
//    m_config.axis.inversions.r_x = m_pin_inverts["r_x"].toBool();
//    m_config.axis.inversions.r_y = m_pin_inverts["r_y"].toBool();
}


