#include "port.h"
#include "QDebug"
#include "QThread"
#include <QProcess>
#include <iostream>
#include <QSettings>
#include <QCoreApplication>
#include <algorithm>
Port::Port(const QSerialPortInfo &serialPortInfo, QObject *parent) : QObject(parent), m_board(ArdwiinoLookup::empty),  m_isReady(false), m_hasPinDetectionCallback(false), readyForRead(false)
{
    rescan(serialPortInfo);
}

Port::Port(QObject *parent) : QObject(parent), m_board(ArdwiinoLookup::empty), m_isReady(false), m_hasPinDetectionCallback(false), readyForRead(false)
{
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

void Port::handleConnection(const QSerialPortInfo& info) {
    if (m_serialPort && !m_serialPort->isOpen()) {
        rescan(info);
        open(info);
    }
}

void Port::rescan(const QSerialPortInfo &serialPortInfo) {
    m_isArdwiino = ArdwiinoLookup::getInstance()->isArdwiino(serialPortInfo);
    m_isOldAPIArdwiino = ArdwiinoLookup::getInstance()->isOldAPIArdwiino(serialPortInfo);
    m_isOutdated = ArdwiinoLookup::getInstance()->isIncompatibleArdwiino(serialPortInfo);
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
            m_description = m_board.name + " - "+m_port;
            boardImageChanged();
            m_isReady = true;
        }
    }
    emit descriptionChanged();
    emit outdatedChanged();
    emit isArdwiinoChanged();
}
uint16_t Port::read_16(QByteArray id) {
    uint16_t* data = (uint16_t*)read(id).data();
    return *data;
}
uint8_t Port::read_single(QByteArray id) {
    return read(id).data()[0] & 0xff;
}
QByteArray Port::read(QByteArray id) {
    m_serialPort->flush();
    m_serialPort->write(id);
    m_serialPort->waitForBytesWritten();
    m_serialPort->waitForReadyRead();
    return m_serialPort->readAll();
}

void Port::findDigital(QJSValue callback) {
    write(QByteArray(1, COMMAND_FIND_DIGITAL));
    m_hasPinDetectionCallback = true;
    m_pinDetectionCallback = callback;
}
void Port::findAnalog(QJSValue callback) {
    write(QByteArray(1, COMMAND_FIND_ANALOG));
    m_hasPinDetectionCallback = true;
    m_pinDetectionCallback = callback;
}
void Port::readData() {
    if (m_isOldAPIArdwiino) {
        do {
            //Sometimes it takes a few readings to start getting real data. Luckily, its rather easy to test if the controller has returned real data or not.
            m_board = ArdwiinoLookup::findByBoard(read(READ_INFO('f')).trimmed().split('-')[0]);
        } while (m_board.name.isEmpty());
        m_description = "Ardwiino - "+ m_board.name+" - Unsupported firmware";
        m_isArdwiino = false;
        m_isOldAPIArdwiino = false;
        emit descriptionChanged();
    } else {
        //If we have an uno, and it is in avrdude mode, send the commands to jump back to ardwiino mode. You can tell it is in avrdude mode as we receive all of the frame commands, including 0x7f.
        if (m_serialPort->readAll().contains(0x7f)) {
            m_serialPort->write("2",1);
            m_serialPort->waitForBytesWritten();
            m_serialPort->setDataTerminalReady(false);
            m_serialPort->clear();
        }
        do {
            //Sometimes it takes a few readings to start getting real data. Luckily, its rather easy to test if the controller has returned real data or not.
            m_board = ArdwiinoLookup::findByBoard(read(READ_INFO(INFO_BOARD)).trimmed());
        } while (m_board.name.isEmpty());
        m_isOutdated |= ArdwiinoLookup::getInstance()->isOld(read(READ_INFO(INFO_VERSION)));
        readyForRead = true;
        if (!m_isOutdated) {
            readAllData();
        }
        readDescription();
    }
    m_hasDFU = m_board.hasDFU;
    dfuFound();
    m_isReady = true;
    boardImageChanged();
    readyChanged();
}
void Port::readAllData() {
    loadPins();
    loadKeys();
    loadLEDs();
    loadMIDI();
    uint8_t id = read_single(READ_CONFIG(CONFIG_SUB_TYPE));
    if (id == REAL_DRUM_SUBTYPE) {
        id = ArdwiinoDefines::XINPUT_GUITAR_HERO_DRUMS;
    }
    if (id == REAL_GUITAR_SUBTYPE) {
        id = ArdwiinoDefines::XINPUT_GUITAR_HERO_GUITAR;
    }
    m_type = ArdwiinoDefines::subtype(id);
    typeChanged();
    m_orientation = ArdwiinoDefines::gyro(read_single(READ_CONFIG(CONFIG_MPU_6050_ORIENTATION)));
    orientationChanged();
    m_input_type = ArdwiinoDefines::input(read_single(READ_CONFIG(CONFIG_INPUT_TYPE)));
    inputTypeChanged();
    m_tilt = ArdwiinoDefines::tilt(read_single(READ_CONFIG(CONFIG_TILT_TYPE)));
    tiltTypeChanged();
    m_led = ArdwiinoDefines::fret_mode(read_single(READ_CONFIG(CONFIG_LED_TYPE)));
    ledTypeChanged();
    m_trigger_threshold = read_single(READ_CONFIG(CONFIG_THRESHOLD_TRIGGER));
    triggerThresholdChanged();
    m_joy_threshold = read_single(READ_CONFIG(CONFIG_THRESHOLD_JOY));
    joyThresholdChanged();
    m_map_joy = read_single(READ_CONFIG(CONFIG_MAP_JOY_DPAD));
    mapJoystickChanged();
    m_map_start_sel_home = read_single(READ_CONFIG(CONFIG_MAP_START_SEL_HOME));
    mapStartSelectHomeChanged();
    m_sensitivity = read_single(READ_CONFIG(CONFIG_TILT_SENSITIVITY));
    tiltSensitivityChanged();

}

void Port::write(QByteArray id) {
    m_serialPort->write(id+QByteArray(1,'\n'));
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
    m_isReady = false;
    readyChanged();
    m_serialPort->flush();
    saveKeys();
    savePins();
    saveLEDs();
    saveMIDI();
    pushWrite(WRITE_CONFIG(CONFIG_INPUT_TYPE, m_input_type));
    pushWrite(WRITE_CONFIG(CONFIG_TILT_TYPE, m_tilt));
    pushWrite(WRITE_CONFIG(CONFIG_MPU_6050_ORIENTATION, m_orientation));
    pushWrite(WRITE_CONFIG(CONFIG_LED_TYPE, m_led));
    pushWrite(WRITE_CONFIG(CONFIG_MAP_JOY_DPAD, m_map_joy));
    pushWrite(WRITE_CONFIG(CONFIG_MAP_START_SEL_HOME, m_map_start_sel_home));
    pushWrite(WRITE_CONFIG(CONFIG_THRESHOLD_TRIGGER, m_trigger_threshold));
    pushWrite(WRITE_CONFIG(CONFIG_THRESHOLD_JOY, m_joy_threshold));
    pushWrite(WRITE_CONFIG(CONFIG_TILT_SENSITIVITY, m_sensitivity));
    pushWrite(QByteArray(1,COMMAND_REBOOT));
    m_serialPort->write(WRITE_CONFIG(CONFIG_SUB_TYPE, m_type));
}
void Port::prepareUpdate() {
    if (board().hasDFU) {
        jump();
    }
}
void Port::jump() {
    m_serialPort->flush();
    m_serialPort->write(QByteArray(1,COMMAND_JUMP_BOOTLOADER));
    m_serialPort->waitForBytesWritten();
    close();
    portStateChanged();
}
void Port::jumpUNO() {
    if (!m_serialPort->isOpen()) {
        m_serialPort->open(QIODevice::ReadWrite);
    }
    m_serialPort->flush();
    m_serialPort->write(QByteArray(1,COMMAND_JUMP_BOOTLOADER_UNO));
    m_serialPort->waitForBytesWritten();
    close();
    portStateChanged();
}
void Port::readDescription() {
    QByteArray readData;
    m_description = "Ardwiino - "+ m_board.name;
    if (m_isOutdated) {
        m_description += " - Outdated";
    } else {
        m_description += " - " + ArdwiinoDefines::getName(m_type);
        uint8_t ext = read_single(READ_INFO(INFO_EXT));
        if (m_input_type == ArdwiinoDefines::WII)  {
            m_description += " - " + ArdwiinoDefines::getName((ArdwiinoDefines::wii_ext_type)ext);
        } else if(m_input_type == ArdwiinoDefines::PS2) {
            m_description += " - " + ArdwiinoDefines::getName((ArdwiinoDefines::PsxControllerType)ext);
        } else {
            m_description += " - " + ArdwiinoDefines::getName(m_input_type);
        }
    }
    m_description += " - " + m_port;

    updateControllerName();
    emit typeChanged();
    emit descriptionChanged();
}

void Port::readyRead() {
    if (m_hasPinDetectionCallback) {
        QTimer::singleShot(0, [this]{
            auto read = m_serialPort->readAll();
            if (read.contains('d')) {
                QJSValueList args;
                args << QJSValue(read[read.indexOf('d')+1] & 0xff);
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
        if (ArdwiinoLookup::is115200(serialPortInfo)) {
            m_serialPort->setBaudRate(QSerialPort::Baud115200);
        } else {
            m_serialPort->setBaudRate(1000000);
        }
        m_serialPort->setDataBits(QSerialPort::DataBits::Data8);
        m_serialPort->setStopBits(QSerialPort::StopBits::OneStop);
        m_serialPort->setParity(QSerialPort::Parity::NoParity);
        m_serialPort->setFlowControl(QSerialPort::FlowControl::HardwareControl);
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
    portStateChanged();
}

void Port::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError != QSerialPort::SerialPortError::NoError && serialPortError != QSerialPort::NotOpenError) {
        qDebug() << serialPortError << m_serialPort->errorString();
        //        m_description = "Ardwiino - Error Communicating";
        //        if (m_serialPort->isOpen()) {
        //            close();
        //            portStateChanged();
        //        }
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
    portStateChanged();
}

void Port::prepareRescan() {
    if (m_serialPort->isOpen()) {
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
    m_keys.clear();
    m_keys["up"] = read_single(READ_CONFIG(CONFIG_KEY_UP));
    m_keys["down"] = read_single(READ_CONFIG(CONFIG_KEY_DOWN));
    m_keys["left"] = read_single(READ_CONFIG(CONFIG_KEY_LEFT));
    m_keys["right"] = read_single(READ_CONFIG(CONFIG_KEY_RIGHT));
    m_keys["start"] = read_single(READ_CONFIG(CONFIG_KEY_START));
    m_keys["back"] = read_single(READ_CONFIG(CONFIG_KEY_SELECT));
    m_keys["left_stick"] = read_single(READ_CONFIG(CONFIG_KEY_LEFT_STICK));
    m_keys["right_stick"] = read_single(READ_CONFIG(CONFIG_KEY_RIGHT_STICK));
    m_keys["LB"] = read_single(READ_CONFIG(CONFIG_KEY_LB));
    m_keys["RB"] = read_single(READ_CONFIG(CONFIG_KEY_RB));
    m_keys["home"] = read_single(READ_CONFIG(CONFIG_KEY_HOME));
    m_keys["capture"] = read_single(READ_CONFIG(CONFIG_KEY_CAPTURE));
    m_keys["a"] = read_single(READ_CONFIG(CONFIG_KEY_A));
    m_keys["b"] = read_single(READ_CONFIG(CONFIG_KEY_B));
    m_keys["x"] = read_single(READ_CONFIG(CONFIG_KEY_X));
    m_keys["y"] = read_single(READ_CONFIG(CONFIG_KEY_Y));
    m_keys["lt"] = read_single(READ_CONFIG(CONFIG_KEY_LT));
    m_keys["rt"] = read_single(READ_CONFIG(CONFIG_KEY_RT));
    auto a = read(READ_CONFIG(CONFIG_KEY_L_X));
    m_keys["l_x_lt"] = a.data()[0];
    m_keys["l_x_gt"] = a.data()[1];
    a = read(READ_CONFIG(CONFIG_KEY_L_Y));
    m_keys["l_y_lt"] = a.data()[0];
    m_keys["l_y_gt"] = a.data()[1];
    a = read(READ_CONFIG(CONFIG_KEY_R_X));
    m_keys["r_x_lt"] = a.data()[0];
    m_keys["r_x_gt"] = a.data()[1];
    a = read(READ_CONFIG(CONFIG_KEY_R_Y));
    m_keys["r_y_lt"] = a.data()[0];
    m_keys["r_y_gt"] = a.data()[1];
}

void Port::saveKeys() {
    pushWrite(WRITE_CONFIG(CONFIG_KEY_UP, uint8_t(m_keys["up"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_DOWN, uint8_t(m_keys["down"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_LEFT, uint8_t(m_keys["left"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_RIGHT, uint8_t(m_keys["right"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_START, uint8_t(m_keys["start"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_SELECT, uint8_t(m_keys["back"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_LEFT_STICK, uint8_t(m_keys["left_stick"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_RIGHT_STICK, uint8_t(m_keys["right_stick"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_LB, uint8_t(m_keys["LB"].toUInt())));

    pushWrite(WRITE_CONFIG(CONFIG_KEY_RB, uint8_t(m_keys["RB"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_HOME, uint8_t(m_keys["home"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_CAPTURE, uint8_t(m_keys["capture"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_A, uint8_t(m_keys["a"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_B, uint8_t(m_keys["b"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_X, uint8_t(m_keys["x"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_Y, uint8_t(m_keys["y"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_LT, uint8_t(m_keys["lt"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_KEY_RT, uint8_t(m_keys["rt"].toUInt())));
    pushWrite(WRITE_CONFIG_PINS(CONFIG_KEY_L_X, uint8_t(m_keys["l_x_lt"].toUInt()),uint8_t(m_keys["l_x_gt"].toUInt())));
    pushWrite(WRITE_CONFIG_PINS(CONFIG_KEY_L_Y, uint8_t(m_keys["l_y_lt"].toUInt()),uint8_t(m_keys["l_y_gt"].toUInt())));
    pushWrite(WRITE_CONFIG_PINS(CONFIG_KEY_R_X, uint8_t(m_keys["r_x_lt"].toUInt()),uint8_t(m_keys["r_x_gt"].toUInt())));
    pushWrite(WRITE_CONFIG_PINS(CONFIG_KEY_R_Y, uint8_t(m_keys["r_y_lt"].toUInt()),uint8_t(m_keys["r_y_gt"].toUInt())));
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
    m_pins["r_y"] = read_single(READ_CONFIG(CONFIG_PIN_R_Y));
    m_pin_inverts["r_y"] = read_single(READ_CONFIG(CONFIG_AXIS_INVERT_R_Y));
}

void Port::savePins() {
    //TODO: ArdwiinoDefines::getInstance()->get_buttons_entries(); should allow us to turn this into a loop, see loadLEDs
    //TODO: however to do this we may need to turn CONFIG_PIN_x into enums.
    pushWrite(WRITE_CONFIG(CONFIG_PIN_UP, uint8_t(m_pins["up"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_DOWN, uint8_t(m_pins["down"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_LEFT, uint8_t(m_pins["left"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_RIGHT, uint8_t(m_pins["right"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_START, uint8_t(m_pins["start"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_SELECT, uint8_t(m_pins["back"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_LEFT_STICK, uint8_t(m_pins["left_stick"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_RIGHT_STICK, uint8_t(m_pins["right_stick"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_LB, uint8_t(m_pins["LB"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_RB, uint8_t(m_pins["RB"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_HOME, uint8_t(m_pins["home"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_A, uint8_t(m_pins["a"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_B, uint8_t(m_pins["b"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_X, uint8_t(m_pins["x"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_Y, uint8_t(m_pins["y"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_LT, uint8_t(m_pins["lt"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_RT, uint8_t(m_pins["rt"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_L_X, uint8_t(m_pins["l_x"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_L_Y, uint8_t(m_pins["l_y"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_R_X, uint8_t(m_pins["r_x"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_AXIS_INVERT_LT, m_pin_inverts["lt"].toBool()));
    pushWrite(WRITE_CONFIG(CONFIG_AXIS_INVERT_RT, m_pin_inverts["rt"].toBool()));
    pushWrite(WRITE_CONFIG(CONFIG_AXIS_INVERT_L_X, m_pin_inverts["l_x"].toBool()));
    pushWrite(WRITE_CONFIG(CONFIG_AXIS_INVERT_L_Y, m_pin_inverts["l_y"].toBool()));
    pushWrite(WRITE_CONFIG(CONFIG_AXIS_INVERT_R_X, m_pin_inverts["r_x"].toBool()));
    pushWrite(WRITE_CONFIG(CONFIG_PIN_R_Y, uint8_t(m_pins["r_y"].toUInt())));
    pushWrite(WRITE_CONFIG(CONFIG_AXIS_INVERT_R_Y, m_pin_inverts["r_y"].toBool()));
}
void Port::loadMIDI() {
    m_midi_note.clear();
    m_midi_type.clear();
    m_midi_channel.clear();
    auto buttons = ArdwiinoDefines::getInstance()->get_buttons_entries();
    QMap<int, QString> inv;
    for (auto k : buttons.keys()) {
        inv[buttons[k].toUInt()] = k;
    }
    for (int i =0; i < buttons.size();i++) {
        uint8_t note = read_single(READ_CONFIG(CONFIG_MIDI_NOTE)+QByteArray(1,i));
        uint8_t type = read_single(READ_CONFIG(CONFIG_MIDI_TYPE)+QByteArray(1,i));
        uint8_t channel = read_single(READ_CONFIG(CONFIG_MIDI_CHANNEL)+QByteArray(1,i));
        m_midi_note[inv[i]] = note;
        m_midi_type[inv[i]] = type;
        m_midi_channel[inv[i]] = channel;
    }
    midiChanged();
}
void Port::saveMIDI() {
    auto buttons = ArdwiinoDefines::getInstance()->get_buttons_entries();
    QMap<int, QString> inv;
    for (auto k : buttons.keys()) {
        inv[buttons[k].toUInt()] = k;
    }
    for (int i =0; i < buttons.size();i++) {
        pushWrite(WRITE_CONFIG_PINS(CONFIG_MIDI_NOTE,i,m_midi_note[inv[i]].toUInt()));
        pushWrite(WRITE_CONFIG_PINS(CONFIG_MIDI_TYPE,i,m_midi_type[inv[i]].toUInt()));
        pushWrite(WRITE_CONFIG_PINS(CONFIG_MIDI_CHANNEL,i,m_midi_channel[inv[i]].toUInt()));
    }
}
void Port::saveLEDs() {
    auto buttons = ArdwiinoDefines::getInstance()->get_buttons_entries();
    for (int i =0; i < buttons.size();i++) {
        int pin = 0;
        if (i < m_leds.size()) {
            QString btn = m_leds[i].toString();
            pin = buttons[btn].toInt()+1;
            uint32_t col = m_colours[btn].toUInt();
            QByteArray colours = WRITE_CONFIG(CONFIG_LED_COLOURS,i)+QByteArray((char*)&col,4);
            pushWrite(colours);
        }
        QByteArray pins = WRITE_CONFIG_PINS(CONFIG_LED_PINS,i,pin);
        pushWrite(pins);
    }
    if (m_dataToWrite.isEmpty()) {
        //We want to make ch led data write immedaitely, as this to a user feels more like a tool config change than a firmware change
        //TODO: honestly, it would make more sense to just write the led colours directly instead of having this map.
        std::vector<QString> keys = {"Note Hit","Star Power","Open Note"};
        for (uint i =0; i < keys.size(); i++) {
            uint32_t col = m_gh_colours[keys[i]].toUInt();
            QByteArray colours = WRITE_CONFIG(CONFIG_LED_GH_COLOURS,i)+QByteArray((char*)&col,4);
            write(colours);
        }
    }
}
void Port::loadLEDs() {
    m_leds.clear();
    m_colours.clear();
    m_gh_colours.clear();
    auto buttons = ArdwiinoDefines::getInstance()->get_buttons_entries();
    QMap<int, QString> inv;
    for (auto k : buttons.keys()) {
        inv[buttons[k].toUInt()] = k;
    }
    for (int i =0; i < buttons.size();i++) {
        uint8_t pin = read_single(READ_CONFIG(CONFIG_LED_PINS)+QByteArray(1,i))-1;
        auto colour = ((uint32_t*)read(READ_CONFIG(CONFIG_LED_COLOURS)+QByteArray(1,i)).data())[0];
        //Stop reading leds when we have read all of them
        if (pin == 255) break;
        m_leds.push_back(inv[pin]);
        m_colours[inv[pin]] = colour;
    }
    std::vector<QString> keys = {"Note Hit","Star Power","Open Note"};
    for (uint i =0; i < keys.size(); i++) {
        m_gh_colours[keys[i]] = QVariant(((uint32_t*)read(READ_CONFIG(CONFIG_LED_GH_COLOURS)+QByteArray(1,i)).data())[0]);
    }
    ledsChanged();
}

