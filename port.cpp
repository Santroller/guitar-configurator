#include "port.h"

#include <QCoreApplication>
#include <QDataStream>
#include <QProcess>
#include <QSettings>
#include <algorithm>
#include <iostream>

#include "QDebug"
#include "QThread"
Port::Port(const QSerialPortInfo &serialPortInfo, QObject *parent) : QObject(parent), m_board(ArdwiinoLookup::empty), m_isReady(false), m_hasPinDetectionCallback(false), readyForRead(false), m_isValid(true) {
    rescan(serialPortInfo);
}
Port::Port(board_t board, QObject *parent) : QObject(parent), m_serialPort(nullptr), m_board(board), m_isReady(false), m_hasPinDetectionCallback(false), readyForRead(false), m_isValid(true) {
    m_description = boardName() + " in restore mode";
    m_port = "dfu";
    m_isReady = true;
    m_isAlreadyDFU = true;
    m_isArdwiino = false;
}
Port::Port(QObject *parent) : QObject(parent), m_board(ArdwiinoLookup::empty), m_isReady(false), m_hasPinDetectionCallback(false), readyForRead(false), m_isValid(false) {
    m_description = "Searching for devices";
    m_port = "searching";
}

void Port::close() {
    readyForRead = false;
    if (m_serialPort != nullptr) {
        m_serialPort->close();
        portStateChanged();
    }
}
void Port::scanAfterDFU() {
    m_isAlreadyDFU = false;
    m_serialPort->setBaudRate(1000000);
    m_serialPort->open(QSerialPort::ReadWrite);
    m_serialPort->write(data_slot(COMMAND_REBOOT));
    m_serialPort->waitForBytesWritten();
    m_serialPort->close();
}
void Port::handleDisconnection(const QSerialPortInfo &info) {
    if (m_serialPort->isOpen() && info.portName() == m_serialPort->portName()) {
        m_serialPort->close();
        m_disconnected = true;
        disconnectedChanged();
    }
}
void Port::handleConnection(const QSerialPortInfo &info) {
    if (m_isAlreadyDFU && m_board.hasDFU) {
        if (m_serialPort == nullptr) {
            m_disconnected = false;
            m_port = info.systemLocation();
            m_serialPort = new QSerialPort(info);
            m_serialPort->setBaudRate(1000000);
            m_serialPort->open(QSerialPort::ReadWrite);
            jump();
            m_serialPort->setBaudRate(115200);
            m_serialPort->open(QSerialPort::ReadWrite);
            jump();
        }
    } else if (m_serialPort && !m_serialPort->isOpen()) {
        rescan(info);
        open(info);
    }
}

void Port::rescan(const QSerialPortInfo &serialPortInfo) {
    m_disconnected = false;
    m_isArdwiino = ArdwiinoLookup::getInstance()->isArdwiino(serialPortInfo);
    m_isOldAPIArdwiino = ArdwiinoLookup::getInstance()->isOldAPIArdwiino(serialPortInfo);
    m_isAlreadyDFU = ArdwiinoLookup::getInstance()->isAreadyDFU(serialPortInfo);
    if (m_isArdwiino) {
        m_description = "Ardwiino - Reading Controller Information";
        m_port = serialPortInfo.systemLocation();
    } else {
        auto b = ArdwiinoLookup::detectBoard(serialPortInfo);
        if (!b.name.isEmpty()) {
            if (m_board.name.isEmpty()) {
                m_board = b;
            }
            m_port = serialPortInfo.systemLocation();
            m_description = m_board.name + " - " + m_port;
            boardImageChanged();
            m_isReady = true;
        }
    }
    emit descriptionChanged();
    emit outdatedChanged();
    emit isArdwiinoChanged();
}
QByteArray Port::data_data(uint8_t slot, uint8_t data) {
    QByteArray a;
    QDataStream ds(&a, QIODevice::ReadWrite);
    ds.setByteOrder(QDataStream::LittleEndian);
    ds << slot;
    ds << data;
    return a;
}
QByteArray Port::data_slot(uint8_t slot) {
    QByteArray a;
    QDataStream ds(&a, QIODevice::ReadWrite);
    ds.setByteOrder(QDataStream::LittleEndian);
    ds << slot;
    return a;
}
QByteArray Port::data_extra(uint8_t slot, uint8_t data, uint8_t extra) {
    QByteArray a;
    QDataStream ds(&a, QIODevice::ReadWrite);
    ds.setByteOrder(QDataStream::LittleEndian);
    ds << slot;
    ds << data;
    ds << extra;
    return a;
}
QByteArray Port::data_extra_pins(uint8_t slot, uint8_t data, uint8_t pin, uint8_t extra) {
    QByteArray a;
    QDataStream ds(&a, QIODevice::ReadWrite);
    ds.setByteOrder(QDataStream::LittleEndian);
    ds << slot;
    ds << data;
    ds << pin;
    ds << extra;
    return a;
}
QByteArray Port::data_extra_pins_32(uint8_t slot, uint8_t data, uint8_t pin, uint32_t extra) {
    QByteArray a;
    QDataStream ds(&a, QIODevice::ReadWrite);
    ds.setByteOrder(QDataStream::LittleEndian);
    ds << slot;
    ds << data;
    ds << pin;
    ds << extra;
    return a;
}
QList<uint8_t> Port::read_8_n(QByteArray a, uint8_t count) {
    QDataStream ds(a);
    QList<uint8_t> list;
    uint8_t tmp;
    for (uint8_t i = 0; i < count; i++) {
        ds >> tmp;
        list << tmp;
    }
    return list;
}
uint8_t Port::read_8(QByteArray a) {
    QDataStream ds(read(a));
    ds.setByteOrder(QDataStream::LittleEndian);
    uint8_t ret;
    ds >> ret;
    return ret;
}
uint16_t Port::read_16(QByteArray a) {
    QDataStream ds(read(a));
    ds.setByteOrder(QDataStream::LittleEndian);
    uint16_t ret;
    ds >> ret;
    return ret;
}
uint32_t Port::read_32(QByteArray a) {
    QDataStream ds(read(a));
    ds.setByteOrder(QDataStream::LittleEndian);
    uint32_t ret;
    ds >> ret;
    return ret;
}
QByteArray Port::read(QByteArray id) {
    m_serialPort->flush();
    m_serialPort->write(id);
    m_serialPort->waitForBytesWritten();
    m_serialPort->waitForReadyRead();
    return m_serialPort->readAll();
}

void Port::findDigital(QJSValue callback) {
    m_hasPinDetectionCallback = true;
    m_pinDetectionCallback = callback;
    writeNoResp(data_slot(COMMAND_FIND_DIGITAL));
}
void Port::findAnalog(QJSValue callback) {
    m_hasPinDetectionCallback = true;
    m_pinDetectionCallback = callback;
    writeNoResp(data_slot(COMMAND_FIND_ANALOG));
}
void Port::readData() {
    if (m_isOldAPIArdwiino) {
        do {
            //Sometimes it takes a few readings to start getting real data. Luckily, its rather easy to test if the controller has returned real data or not.
            m_board = ArdwiinoLookup::findByBoard(read(data_data('i', 'f')).trimmed().split('-')[0]);
        } while (m_board.name.isEmpty());
        m_description = "Ardwiino - " + m_board.name + " - Unsupported firmware";
        m_isArdwiino = false;
        m_isOldAPIArdwiino = false;
        emit descriptionChanged();
    } else {
        //If we have an uno, and it is in avrdude mode, send the commands to jump back to ardwiino mode. You can tell it is in avrdude mode as we receive all of the frame commands, including 0x7f.
        if (m_serialPort->readAll().contains(0x7f)) {
            m_serialPort->write("2", 1);
            m_serialPort->waitForBytesWritten();
            m_serialPort->setDataTerminalReady(false);
            m_serialPort->clear();
        }
        do {
            //Sometimes it takes a few readings to start getting real data. Luckily, its rather easy to test if the controller has returned real data or not.
            m_board = ArdwiinoLookup::findByBoard(read(data_data('i', 6)).trimmed());
        } while (m_board.name.isEmpty());
        readyForRead = true;
        setBoard(m_board.shortName, read(data_data('i', 4)).trimmed().replace("UL", "").toInt());
        readDescription();
    }
    m_hasDFU = m_board.hasDFU;
    dfuFound();
    m_isReady = true;
    boardImageChanged();
    readyChanged();
}
void Port::readAllData() {
    // loadPins();
    // loadKeys();
    // loadLEDs();
    // loadMIDI();
    // uint8_t id = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_SUB_TYPE));
    // if (id == REAL_DRUM_SUBTYPE) {
    //     id = ArdwiinoDefines::XINPUT_GUITAR_HERO_DRUMS;
    // }
    // if (id == REAL_GUITAR_SUBTYPE) {
    //     id = ArdwiinoDefines::XINPUT_GUITAR_HERO_GUITAR;
    // }
    // m_type = ArdwiinoDefines::SubType(id);
    // typeChanged();
    // m_orientation = ArdwiinoDefines::GyroOrientation(read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_MPU_6050_ORIENTATION)));
    // orientationChanged();
    // m_input_type = ArdwiinoDefines::InputType(read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_INPUT_TYPE)));
    // inputTypeChanged();
    // m_tilt = ArdwiinoDefines::TiltType(read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_TILT_TYPE)));
    // tiltTypeChanged();
    // m_led = ArdwiinoDefines::FretLedMode(read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_LED_TYPE)));
    // ledTypeChanged();
    // m_trigger_threshold = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_THRESHOLD_TRIGGER));
    // triggerThresholdChanged();
    // m_joy_threshold = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_THRESHOLD_JOY));
    // joyThresholdChanged();
    // m_map_joy = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_MAP_JOY_DPAD));
    // mapJoystickChanged();
    // m_map_start_sel_home = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_MAP_START_SEL_HOME));
    // mapStartSelectHomeChanged();
    // m_sensitivity = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_TILT_SENSITIVITY));
    // tiltSensitivityChanged();
}

void Port::write(QByteArray id) {
    m_serialPort->write(id);
    m_serialPort->waitForBytesWritten();
    m_serialPort->waitForReadyRead();
    m_serialPort->readLine();
}
void Port::pushWrite(QByteArray id) {
    m_dataToWrite.enqueue(id);
}

void Port::writeNoResp(QByteArray id) {
    m_serialPort->write(id);
    m_serialPort->waitForBytesWritten();
}
void Port::writeConfig() {
    // m_isReady = false;
    // readyChanged();
    // m_serialPort->flush();
    // saveKeys();
    // savePins();
    // saveLEDs();
    // saveMIDI();
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_INPUT_TYPE, m_input_type));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_TILT_TYPE, m_tilt));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_MPU_6050_ORIENTATION, m_orientation));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_LED_TYPE, m_led));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_MAP_JOY_DPAD, m_map_joy));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_MAP_START_SEL_HOME, m_map_start_sel_home));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_THRESHOLD_TRIGGER, m_trigger_threshold));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_THRESHOLD_JOY, m_joy_threshold));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_TILT_SENSITIVITY, m_sensitivity));
    // pushWrite(data_slot(COMMAND_REBOOT));
    // m_serialPort->write(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_SUB_TYPE, m_type));
}
void Port::prepareUpdate() {
    if (board().hasDFU) {
        jump();
    }
}
void Port::jump() {
    // m_serialPort->flush();
    // m_serialPort->write(data_slot(COMMAND_JUMP_BOOTLOADER));
    // m_serialPort->waitForBytesWritten();
    // close();
    // portStateChanged();
}
void Port::jumpUNO() {
    // if (!m_serialPort->isOpen()) {
    //     m_serialPort->open(QIODevice::ReadWrite);
    // }
    // m_serialPort->flush();
    // m_serialPort->write(data_slot(COMMAND_JUMP_BOOTLOADER_UNO));
    // m_serialPort->waitForBytesWritten();
    // close();
    // portStateChanged();
}
void Port::readDescription() {
    QByteArray readData;
    m_description = "Ardwiino - " + m_board.name;
    // if (m_isOutdated) {
    m_description += " - Outdated";
    // } else {
    //     m_description += " - " + ArdwiinoDefines::getName(m_type);
    //     if (m_input_type == ArdwiinoDefines::WII) {
    //         uint16_t ext = read_16(data_data(COMMAND_READ_INFO, INFO_EXT));
    //         auto extName = ArdwiinoDefines::getName((ArdwiinoDefines::WiiExtType)ext);
    //         if (extName == "Unknown")
    //             extName = "Wii Unknown Extension";
    //         m_description += " - " + extName;
    //     } else if (m_input_type == ArdwiinoDefines::PS2) {
    //         uint8_t ext = read_8(data_data(COMMAND_READ_INFO, INFO_EXT));
    //         m_description += " - " + ArdwiinoDefines::getName((ArdwiinoDefines::PsxControllerType)ext);
    //     } else {
    //         m_description += " - " + ArdwiinoDefines::getName(m_input_type);
    //     }
    // }
    m_description += " - " + m_port;

    updateControllerName();
    emit typeChanged();
    emit descriptionChanged();
}

void Port::readyRead() {
    if (m_hasPinDetectionCallback) {
        QTimer::singleShot(0, [this] {
            auto read = m_serialPort->readAll();
            if (read.contains('d')) {
                QJSValueList args;
                args << QJSValue(read[read.indexOf('d') + 1] & 0xff);
                m_pinDetectionCallback.call(args);
                m_hasPinDetectionCallback = false;
                detectedPinChanged();
            }
        });
    }
    if (!m_dataToWrite.isEmpty()) {
        m_serialPort->flush();
        m_serialPort->readAll();
        m_serialPort->write(m_dataToWrite.dequeue());
        if (m_dataToWrite.isEmpty()) {
            m_serialPort->waitForBytesWritten();
            close();
            portStateChanged();
        }
    }
}
void Port::open(const QSerialPortInfo &serialPortInfo) {
    m_serialPort = new QSerialPort(serialPortInfo);
    connect(m_serialPort, &QSerialPort::readyRead, this, &Port::readyRead);
    if (m_isArdwiino) {
        m_serialPort->setBaudRate(QSerialPort::Baud115200);
        m_serialPort->setDataBits(QSerialPort::DataBits::Data8);
        m_serialPort->setStopBits(QSerialPort::StopBits::OneStop);
        m_serialPort->setParity(QSerialPort::Parity::NoParity);
        m_serialPort->setFlowControl(QSerialPort::FlowControl::HardwareControl);
        QObject::connect(m_serialPort, &QSerialPort::errorOccurred, this, &Port::handleError);
        if (m_serialPort->open(QIODevice::ReadWrite)) {
            QThread *thread = QThread::create([this] {
                //We need a delay on linux for unos, as the 328p gets rebooted when we open.
                QThread::msleep(2000);
                QMetaObject::invokeMethod(this, [this] { readData(); });
            });
            thread->start();
        }
    }
    portStateChanged();
}

void Port::handleError(QSerialPort::SerialPortError serialPortError) {
    if (serialPortError != QSerialPort::SerialPortError::NoError && serialPortError != QSerialPort::NotOpenError) {
        qDebug() << serialPortError << m_serialPort->errorString();
    }
}
void Port::touch() {
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
void Port::prepareUpload() {
    if (m_board.protocol == "avr109") {
        touch();
    } else if (m_board.protocol == "arduino") {
        if (!m_serialPort->isOpen()) {
            m_serialPort->open(QIODevice::WriteOnly);
        }
        m_serialPort->write(data_slot(COMMAND_JUMP_BOOTLOADER_UNO));
        m_serialPort->waitForBytesWritten();
        close();
    }
    portStateChanged();
}

void Port::prepareRescan() {
    if (m_serialPort && m_serialPort->isOpen()) {
        close();
    }
    portStateChanged();
}

void Port::updateControllerName() {
    //On windows, update the controller name, so that users see the current device name in games
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\System\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_1209&PID_2882", QSettings::NativeFormat);
    settings.setValue("OEMName", m_description);
#endif
}
void Port::loadKeys() {
    // m_keys.clear();
    // m_keys["up"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_UP));
    // m_keys["down"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_DOWN));
    // m_keys["left"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_LEFT));
    // m_keys["right"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_RIGHT));
    // m_keys["start"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_START));
    // m_keys["back"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_SELECT));
    // m_keys["left_stick"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_LEFT_STICK));
    // m_keys["right_stick"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_RIGHT_STICK));
    // m_keys["LB"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_LB));
    // m_keys["RB"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_RB));
    // m_keys["home"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_HOME));
    // m_keys["capture"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_CAPTURE));
    // m_keys["a"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_A));
    // m_keys["b"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_B));
    // m_keys["x"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_X));
    // m_keys["y"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_Y));
    // m_keys["lt"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_LT));
    // m_keys["rt"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_RT));
    // auto a = read_8_n(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_L_X), 2);
    // m_keys["l_x_lt"] = a[0];
    // m_keys["l_x_gt"] = a[1];
    // a = read_8_n(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_L_Y), 2);
    // m_keys["l_y_lt"] = a[0];
    // m_keys["l_y_gt"] = a[1];
    // a = read_8_n(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_R_X), 2);
    // m_keys["r_x_lt"] = a[0];
    // m_keys["r_x_gt"] = a[1];
    // a = read_8_n(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_KEY_R_Y), 2);
    // m_keys["r_y_lt"] = a[0];
    // m_keys["r_y_gt"] = a[1];
}

void Port::saveKeys() {
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_UP, uint8_t(m_keys["up"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_DOWN, uint8_t(m_keys["down"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_LEFT, uint8_t(m_keys["left"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_RIGHT, uint8_t(m_keys["right"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_START, uint8_t(m_keys["start"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_SELECT, uint8_t(m_keys["back"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_LEFT_STICK, uint8_t(m_keys["left_stick"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_RIGHT_STICK, uint8_t(m_keys["right_stick"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_LB, uint8_t(m_keys["LB"].toUInt())));

    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_RB, uint8_t(m_keys["RB"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_HOME, uint8_t(m_keys["home"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_CAPTURE, uint8_t(m_keys["capture"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_A, uint8_t(m_keys["a"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_B, uint8_t(m_keys["b"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_X, uint8_t(m_keys["x"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_Y, uint8_t(m_keys["y"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_LT, uint8_t(m_keys["lt"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_RT, uint8_t(m_keys["rt"].toUInt())));
    // pushWrite(data_extra_pins(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_L_X, uint8_t(m_keys["l_x_lt"].toUInt()), uint8_t(m_keys["l_x_gt"].toUInt())));
    // pushWrite(data_extra_pins(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_L_Y, uint8_t(m_keys["l_y_lt"].toUInt()), uint8_t(m_keys["l_y_gt"].toUInt())));
    // pushWrite(data_extra_pins(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_R_X, uint8_t(m_keys["r_x_lt"].toUInt()), uint8_t(m_keys["r_x_gt"].toUInt())));
    // pushWrite(data_extra_pins(COMMAND_WRITE_CONFIG_VALUE, CONFIG_KEY_R_Y, uint8_t(m_keys["r_y_lt"].toUInt()), uint8_t(m_keys["r_y_gt"].toUInt())));
}

void Port::loadPins() {
    m_pins.clear();
    // m_pins["up"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_UP));
    // m_pins["down"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_DOWN));
    // m_pins["left"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_LEFT));
    // m_pins["right"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_RIGHT));
    // m_pins["start"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_START));
    // m_pins["back"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_SELECT));
    // m_pins["left_stick"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_LEFT_STICK));
    // m_pins["right_stick"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_RIGHT_STICK));
    // m_pins["LB"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_LB));
    // m_pins["RB"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_RB));
    // m_pins["home"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_HOME));
    // m_pins["capture"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_CAPTURE));
    // m_pins["a"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_A));
    // m_pins["b"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_B));
    // m_pins["x"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_X));
    // m_pins["y"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_Y));
    // m_pins["lt"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_LT));
    // m_pins["rt"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_RT));
    // m_pins["l_x"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_L_X));
    // m_pins["l_y"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_L_Y));
    // m_pins["r_x"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_R_X));
    // m_pins["r_y"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_R_Y));
    // m_pin_inverts["lt"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_AXIS_INVERT_LT));
    // m_pin_inverts["rt"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_AXIS_INVERT_RT));
    // m_pin_inverts["l_x"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_AXIS_INVERT_L_X));
    // m_pin_inverts["l_y"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_AXIS_INVERT_L_Y));
    // m_pin_inverts["r_x"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_AXIS_INVERT_R_X));
    // m_pins["r_y"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_PIN_R_Y));
    // m_pin_inverts["r_y"] = read_8(data_data(COMMAND_READ_CONFIG_VALUE, CONFIG_AXIS_INVERT_R_Y));
}

void Port::savePins() {
    //TODO: ArdwiinoDefines::getInstance()->get_buttons_entries(); should allow us to turn this into a loop, see loadLEDs
    //TODO: however to do this we may need to turn CONFIG_PIN_x into enums.
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_UP, uint8_t(m_pins["up"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_DOWN, uint8_t(m_pins["down"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_LEFT, uint8_t(m_pins["left"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_RIGHT, uint8_t(m_pins["right"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_START, uint8_t(m_pins["start"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_SELECT, uint8_t(m_pins["back"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_LEFT_STICK, uint8_t(m_pins["left_stick"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_RIGHT_STICK, uint8_t(m_pins["right_stick"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_LB, uint8_t(m_pins["LB"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_RB, uint8_t(m_pins["RB"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_HOME, uint8_t(m_pins["home"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_A, uint8_t(m_pins["a"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_B, uint8_t(m_pins["b"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_X, uint8_t(m_pins["x"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_Y, uint8_t(m_pins["y"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_LT, uint8_t(m_pins["lt"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_RT, uint8_t(m_pins["rt"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_L_X, uint8_t(m_pins["l_x"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_L_Y, uint8_t(m_pins["l_y"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_R_X, uint8_t(m_pins["r_x"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_AXIS_INVERT_LT, m_pin_inverts["lt"].toBool()));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_AXIS_INVERT_RT, m_pin_inverts["rt"].toBool()));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_AXIS_INVERT_L_X, m_pin_inverts["l_x"].toBool()));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_AXIS_INVERT_L_Y, m_pin_inverts["l_y"].toBool()));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_AXIS_INVERT_R_X, m_pin_inverts["r_x"].toBool()));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_PIN_R_Y, uint8_t(m_pins["r_y"].toUInt())));
    // pushWrite(data_extra(COMMAND_WRITE_CONFIG_VALUE, CONFIG_AXIS_INVERT_R_Y, m_pin_inverts["r_y"].toBool()));
}
void Port::loadMIDI() {
    // m_midi_note.clear();
    // m_midi_type.clear();
    // m_midi_channel.clear();
    // auto buttons = ArdwiinoDefines::getInstance()->get_buttons_entries();
    // QMap<int, QString> inv;
    // for (auto k : buttons.keys()) {
    //     inv[buttons[k].toUInt()] = k;
    // }
    // for (int i = 0; i < buttons.size(); i++) {
    //     // uint8_t note = read_8(data_extra(COMMAND_READ_CONFIG_VALUE, CONFIG_MIDI_NOTE, i));
    //     // uint8_t type = read_8(data_extra(COMMAND_READ_CONFIG_VALUE, CONFIG_MIDI_TYPE, i));
    //     // uint8_t channel = read_8(data_extra(COMMAND_READ_CONFIG_VALUE, CONFIG_MIDI_CHANNEL, i));
    //     m_midi_note[inv[i]] = note;
    //     m_midi_type[inv[i]] = type;
    //     m_midi_channel[inv[i]] = channel;
    // }
    // midiChanged();
}
void Port::saveMIDI() {
    // auto buttons = ArdwiinoDefines::getInstance()->get_buttons_entries();
    // QMap<int, QString> inv;
    // for (auto k : buttons.keys()) {
    //     inv[buttons[k].toUInt()] = k;
    // }
    // for (int i = 0; i < buttons.size(); i++) {
    //     // pushWrite(data_extra_pins(COMMAND_WRITE_CONFIG_VALUE, CONFIG_MIDI_NOTE, i, m_midi_note[inv[i]].toUInt()));
    //     // pushWrite(data_extra_pins(COMMAND_WRITE_CONFIG_VALUE, CONFIG_MIDI_TYPE, i, m_midi_type[inv[i]].toUInt()));
    //     // pushWrite(data_extra_pins(COMMAND_WRITE_CONFIG_VALUE, CONFIG_MIDI_CHANNEL, i, m_midi_channel[inv[i]].toUInt()));
    // }
}
void Port::saveLEDs() {
    // auto buttons = ArdwiinoDefines::getInstance()->get_buttons_entries();
    // int i;
    // for (i = 0; i < buttons.size(); i++) {
    //     int pin = 0;
    //     if (i < m_leds.size()) {
    //         QString btn = m_leds[i].toString();
    //         pin = buttons[btn].toInt() + 1;
    //         uint32_t col = m_colours[btn].toUInt();
    //         // QByteArray colours = data_extra_pins_32(COMMAND_WRITE_CONFIG_VALUE, CONFIG_LED_COLOURS, i, col);
    //         // pushWrite(colours);
    //     }
    //     // QByteArray pins = data_extra_pins(COMMAND_WRITE_CONFIG_VALUE, CONFIG_LED_PINS, i, pin);
    //     // pushWrite(pins);
    // }
}
void Port::loadLEDs() {
    // m_leds.clear();
    // m_colours.clear();
    // auto buttons = ArdwiinoDefines::getInstance()->get_buttons_entries();
    // QMap<int, QString> inv;
    // for (auto k : buttons.keys()) {
    //     inv[buttons[k].toUInt()] = k;
    // }
    // for (int i = 0; i < buttons.size(); i++) {
    //     // uint8_t pin = read_8(data_extra(COMMAND_READ_CONFIG_VALUE, CONFIG_LED_PINS, i)) - 1;
    //     // auto colour = read_32(data_extra(COMMAND_READ_CONFIG_VALUE, CONFIG_LED_COLOURS, i));
    //     // //Stop reading leds when we have read all of them
    //     // if (pin == 255)
    //     //     break;
    //     // m_leds.push_back(inv[pin]);
    //     // m_colours[inv[pin]] = colour;
    // }
    // ledsChanged();
}
