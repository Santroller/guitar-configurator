#include "port.h"
#include "QDebug"
#include "QThread"
#include <QProcess>
#include "input_types.h"
#include <iostream>
#include <QSettings>
Port::Port(const QSerialPortInfo &serialPortInfo, QObject *parent) : QObject(parent), m_board(ArdwiinoLookup::empty), m_isOldArdwiino(false), m_isReady(false), readyForRead(false)
{
    rescan(serialPortInfo);
}

Port::Port(QObject *parent) : QObject(parent), m_board(ArdwiinoLookup::empty), m_isOldArdwiino(false), m_isReady(false), readyForRead(false)
{
    m_description = "Searching for devices";
    m_port = "searching";
}

void Port::close() {
    readyForRead = false;
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
    m_isOldArdwiino = ArdwiinoLookup::getInstance()->isOldFirmwareArdwiino(serialPortInfo);
    m_isOutdated = ArdwiinoLookup::getInstance()->isOldArdwiino(serialPortInfo);
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
            m_isReady = true;
        }
    }
    emit descriptionChanged(m_description);
    emit outdatedChanged();
    emit isArdwiinoChanged();
}
uint8_t Port::read_single(QByteArray id) {
    return read(id).data()[0] & 0xff;
}
QByteArray Port::read(QByteArray id) {
    m_serialPort->flush();
    m_serialPort->write(id);
    m_serialPort->waitForBytesWritten();
    m_serialPort->waitForReadyRead();
    return m_serialPort->readLine();
}
void Port::readData() {
    if (m_isOldArdwiino) {
        do {
            //Sometimes it takes a few readings to start getting real data. Luckily, its rather easy to test if the controller has returned real data or not.
            m_board = ArdwiinoLookup::findByBoard(read(READ_INFO('f')).trimmed().split('-')[0]);
        } while (m_board.name.isEmpty());
        m_description = "Ardwiino - "+ m_board.name+" - Unsupported firmware";
        m_isArdwiino = false;
        m_isOldArdwiino = false;
        emit descriptionChanged(m_description);
    } else {
        do {
            //Sometimes it takes a few readings to start getting real data. Luckily, its rather easy to test if the controller has returned real data or not.
            m_board = ArdwiinoLookup::findByBoard(read(READ_INFO(INFO_BOARD)).trimmed());
        } while (m_board.name.isEmpty());
        readyForRead = true;
        readDescription();
    }
    m_hasDFU = m_board.hasDFU;
    dfuFound(m_hasDFU);
    m_isReady = true;
    boardImageChanged(getBoardImage());
    readyChanged();
}
void Port::write(QByteArray id) {
    m_serialPort->write(id);
    m_serialPort->waitForBytesWritten();
    m_serialPort->waitForReadyRead();
    m_serialPort->readLine();
}

void Port::startConfiguring() {
    write(QByteArray(1,COMMAND_START_CONFIG));
}

void Port::writeConfig() {
    m_serialPort->flush();
    m_serialPort->write(QByteArray(1,COMMAND_APPLY_CONFIG));
    m_serialPort->waitForBytesWritten();
    close();
    portStateChanged(getOpen());
}
void Port::jump() {
    m_serialPort->flush();
    m_serialPort->write(QByteArray(1,COMMAND_JUMP_BOOTLOADER));
    m_serialPort->waitForBytesWritten();
    close();
    portStateChanged(getOpen());
}
void Port::jumpUNO() {
    m_serialPort->flush();
    m_serialPort->write(QByteArray(1,COMMAND_JUMP_BOOTLOADER_UNO));
    m_serialPort->waitForBytesWritten();
    close();
    portStateChanged(getOpen());
}
void Port::readDescription() {
    QByteArray readData;
    auto vtype = InputTypes::Value(read_single(READ_CONFIG(CONFIG_INPUT_TYPE)));
    auto ctype = Controllers::Value(read_single(READ_CONFIG(CONFIG_SUB_TYPE)));
    m_description = "Ardwiino - "+ m_board.name;
    if (m_isOutdated) {
        m_description += " - Outdated";
    }
    m_description += " - "+Controllers::toString(ctype);
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
            close();
            portStateChanged(getOpen());
        }
    }
}
void Port::prepareUpload() {
    if (m_board.protocol == "avr109") {
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
    } else if (m_board.protocol == "arduino") {
        if (!m_serialPort->isOpen()) {
            m_serialPort->open(QIODevice::WriteOnly);
        }
        m_serialPort->write(QByteArray(1, COMMAND_JUMP_BOOTLOADER_UNO));
        m_serialPort->waitForBytesWritten();
        close();
    }
    portStateChanged(getOpen());
}

void Port::prepareRescan() {
    if (m_serialPort->isOpen()) {
        close();
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
    m_pins["up"] = read_single(READ_CONFIG(CONFIG_KEY_UP));
    m_pins["down"] = read_single(READ_CONFIG(CONFIG_KEY_DOWN));
    m_pins["left"] = read_single(READ_CONFIG(CONFIG_KEY_LEFT));
    m_pins["right"] = read_single(READ_CONFIG(CONFIG_KEY_RIGHT));
    m_pins["start"] = read_single(READ_CONFIG(CONFIG_KEY_START));
    m_pins["back"] = read_single(READ_CONFIG(CONFIG_KEY_SELECT));
    m_pins["left_stick"] = read_single(READ_CONFIG(CONFIG_KEY_LEFT_STICK));
    m_pins["right_stick"] = read_single(READ_CONFIG(CONFIG_KEY_RIGHT_STICK));
    m_pins["LB"] = read_single(READ_CONFIG(CONFIG_KEY_LB));
    m_pins["RB"] = read_single(READ_CONFIG(CONFIG_KEY_RB));
    m_pins["home"] = read_single(READ_CONFIG(CONFIG_KEY_HOME));
    m_pins["capture"] = read_single(READ_CONFIG(CONFIG_KEY_CAPTURE));
    m_pins["a"] = read_single(READ_CONFIG(CONFIG_KEY_A));
    m_pins["b"] = read_single(READ_CONFIG(CONFIG_KEY_B));
    m_pins["x"] = read_single(READ_CONFIG(CONFIG_KEY_X));
    m_pins["y"] = read_single(READ_CONFIG(CONFIG_KEY_Y));
    m_pins["lt"] = read_single(READ_CONFIG(CONFIG_KEY_LT));
    m_pins["rt"] = read_single(READ_CONFIG(CONFIG_KEY_RT));
    auto a = read(READ_CONFIG(CONFIG_KEY_L_X));
    m_pins["l_x_lt"] = a.data()[0];
    m_pins["l_x_gt"] = a.data()[1];
    a = read(READ_CONFIG(CONFIG_KEY_L_Y));
    m_pins["l_y_lt"] = a.data()[0];
    m_pins["l_y_gt"] = a.data()[1];
    a = read(READ_CONFIG(CONFIG_KEY_R_X));
    m_pins["r_x_lt"] = a.data()[0];
    m_pins["r_x_gt"] = a.data()[1];
    a = read(READ_CONFIG(CONFIG_KEY_R_Y));
    m_pins["r_y_lt"] = a.data()[0];
    m_pins["r_y_gt"] = a.data()[1];
}

void Port::saveKeys() {
    write(WRITE_CONFIG(CONFIG_KEY_UP, uint8_t(m_pins["up"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_DOWN, uint8_t(m_pins["down"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_LEFT, uint8_t(m_pins["left"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_RIGHT, uint8_t(m_pins["right"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_START, uint8_t(m_pins["start"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_SELECT, uint8_t(m_pins["back"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_LEFT_STICK, uint8_t(m_pins["left_stick"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_RIGHT_STICK, uint8_t(m_pins["right_stick"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_LB, uint8_t(m_pins["LB"].toUInt())));

    write(WRITE_CONFIG(CONFIG_KEY_RB, uint8_t(m_pins["RB"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_HOME, uint8_t(m_pins["home"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_CAPTURE, uint8_t(m_pins["capture"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_A, uint8_t(m_pins["a"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_B, uint8_t(m_pins["b"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_X, uint8_t(m_pins["x"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_Y, uint8_t(m_pins["y"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_LT, uint8_t(m_pins["lt"].toUInt())));
    write(WRITE_CONFIG(CONFIG_KEY_RT, uint8_t(m_pins["rt"].toUInt())));
    write(WRITE_CONFIG_PINS(CONFIG_KEY_L_X, uint8_t(m_pins["l_x_lt"].toUInt()),uint8_t(m_pins["l_x_gt"].toUInt())));
    write(WRITE_CONFIG_PINS(CONFIG_KEY_L_Y, uint8_t(m_pins["l_y_lt"].toUInt()),uint8_t(m_pins["l_y_gt"].toUInt())));
    write(WRITE_CONFIG_PINS(CONFIG_KEY_R_X, uint8_t(m_pins["r_x_lt"].toUInt()),uint8_t(m_pins["r_x_gt"].toUInt())));
    write(WRITE_CONFIG_PINS(CONFIG_KEY_R_Y, uint8_t(m_pins["r_y_lt"].toUInt()),uint8_t(m_pins["r_y_gt"].toUInt())));
}

void Port::loadPins() {
    m_pins.clear();
    m_pins["up"] = read_single(READ_CONFIG(CONFIG_PIN_UP));
    m_pins["down"] = read_single(READ_CONFIG(CONFIG_PIN_DOWN));
    m_pins["left"] = read_single(READ_CONFIG(CONFIG_PIN_LEFT));
    m_pins["right"] = read_single(READ_CONFIG(CONFIG_PIN_RIGHT));
    m_pins["start"] = read_single(READ_CONFIG(CONFIG_PIN_START));
    m_pins["back"] = read_single(READ_CONFIG(CONFIG_PIN_SELECT));
    m_pins["left_stick"] = read_single(READ_CONFIG(CONFIG_PIN_LEFT_STICK));
    m_pins["right_stick"] = read_single(READ_CONFIG(CONFIG_PIN_RIGHT_STICK));
    m_pins["LB"] = read_single(READ_CONFIG(CONFIG_PIN_LB));
    m_pins["RB"] = read_single(READ_CONFIG(CONFIG_PIN_RB));
    m_pins["home"] = read_single(READ_CONFIG(CONFIG_PIN_HOME));
    m_pins["capture"] = read_single(READ_CONFIG(CONFIG_PIN_CAPTURE));
    m_pins["a"] = read_single(READ_CONFIG(CONFIG_PIN_A));
    m_pins["b"] = read_single(READ_CONFIG(CONFIG_PIN_B));
    m_pins["x"] = read_single(READ_CONFIG(CONFIG_PIN_X));
    m_pins["y"] = read_single(READ_CONFIG(CONFIG_PIN_Y));
    m_pins["lt"] = read_single(READ_CONFIG(CONFIG_PIN_LT));
    m_pins["rt"] = read_single(READ_CONFIG(CONFIG_PIN_RT));
    m_pins["l_x"] = read_single(READ_CONFIG(CONFIG_PIN_L_X));
    m_pins["l_y"] = read_single(READ_CONFIG(CONFIG_PIN_L_Y));
    m_pins["r_x"] = read_single(READ_CONFIG(CONFIG_PIN_R_X));
    m_pins["r_y"] = read_single(READ_CONFIG(CONFIG_PIN_R_Y));
    m_pin_inverts["lt"] = read_single(READ_CONFIG(CONFIG_AXIS_INVERT_LT));
    m_pin_inverts["rt"] = read_single(READ_CONFIG(CONFIG_AXIS_INVERT_RT));
    m_pin_inverts["l_x"] = read_single(READ_CONFIG(CONFIG_AXIS_INVERT_L_X));
    m_pin_inverts["l_y"] = read_single(READ_CONFIG(CONFIG_AXIS_INVERT_L_Y));
    m_pin_inverts["r_x"] = read_single(READ_CONFIG(CONFIG_AXIS_INVERT_R_X));
    m_pin_inverts["r_y"] = read_single(READ_CONFIG(CONFIG_AXIS_INVERT_R_Y));
}

void Port::savePins() {
    write(WRITE_CONFIG(CONFIG_PIN_UP, uint8_t(m_pins["up"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_DOWN, uint8_t(m_pins["down"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_LEFT, uint8_t(m_pins["left"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_RIGHT, uint8_t(m_pins["right"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_START, uint8_t(m_pins["start"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_SELECT, uint8_t(m_pins["back"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_LEFT_STICK, uint8_t(m_pins["left_stick"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_RIGHT_STICK, uint8_t(m_pins["right_stick"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_LB, uint8_t(m_pins["LB"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_RB, uint8_t(m_pins["RB"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_HOME, uint8_t(m_pins["home"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_A, uint8_t(m_pins["a"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_B, uint8_t(m_pins["b"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_X, uint8_t(m_pins["x"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_Y, uint8_t(m_pins["y"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_LT, uint8_t(m_pins["lt"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_RT, uint8_t(m_pins["rt"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_L_X, uint8_t(m_pins["l_x"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_L_Y, uint8_t(m_pins["l_y"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_R_X, uint8_t(m_pins["r_x"].toUInt())));
    write(WRITE_CONFIG(CONFIG_PIN_R_Y, uint8_t(m_pins["r_y"].toUInt())));
    write(WRITE_CONFIG(CONFIG_AXIS_INVERT_LT, m_pin_inverts["lt"].toBool()));
    write(WRITE_CONFIG(CONFIG_AXIS_INVERT_RT, m_pin_inverts["rt"].toBool()));
    write(WRITE_CONFIG(CONFIG_AXIS_INVERT_L_X, m_pin_inverts["l_x"].toBool()));
    write(WRITE_CONFIG(CONFIG_AXIS_INVERT_L_Y, m_pin_inverts["l_y"].toBool()));
    write(WRITE_CONFIG(CONFIG_AXIS_INVERT_R_X, m_pin_inverts["r_x"].toBool()));
    write(WRITE_CONFIG(CONFIG_AXIS_INVERT_R_Y, m_pin_inverts["r_y"].toBool()));
}


