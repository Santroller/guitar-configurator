#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "ardwiinolookup.h"
#include "submodules/Ardwiino/src/shared/output/serial_commands.h"
#include <math.h>
#include "ardwiino_defines.h"
#include <QJSValue>
#include <QQueue>
class Port : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(bool isArdwiino READ isArdwiino NOTIFY isArdwiinoChanged)
    Q_PROPERTY(bool isOutdated READ isOutdated NOTIFY outdatedChanged)
    Q_PROPERTY(bool isGuitar READ isGuitar NOTIFY typeChanged)
    Q_PROPERTY(bool isDrum READ isDrum NOTIFY typeChanged)
    Q_PROPERTY(bool isLiveGuitar READ isLiveGuitar NOTIFY typeChanged)
    Q_PROPERTY(bool isKeyboard READ isKeyboard NOTIFY typeChanged)
    Q_PROPERTY(bool isRB READ isRB NOTIFY typeChanged)
    Q_PROPERTY(bool isWii READ isWii NOTIFY inputTypeChanged)
    Q_PROPERTY(bool isMIDI READ isMIDI NOTIFY typeChanged)
    Q_PROPERTY(bool isMouse READ isMouse NOTIFY typeChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(bool disconnected READ isDisconnected NOTIFY disconnectedChanged)
    Q_PROPERTY(bool hasAddressableLEDs READ hasAddressableLEDs NOTIFY ledTypeChanged)
    Q_PROPERTY(QVariantMap pin_inverts MEMBER m_pin_inverts NOTIFY pinInvertsChanged)
    Q_PROPERTY(QVariantMap pins MEMBER m_pins NOTIFY pinsChanged)
    Q_PROPERTY(QVariantMap keys MEMBER m_keys NOTIFY pinsChanged)
    Q_PROPERTY(QVariantMap midi_type MEMBER m_midi_type NOTIFY midiChanged)
    Q_PROPERTY(QVariantMap midi_note MEMBER m_midi_note NOTIFY midiChanged)
    Q_PROPERTY(QVariantMap midi_channel MEMBER m_midi_channel NOTIFY midiChanged)
    Q_PROPERTY(QVariantList leds MEMBER m_leds NOTIFY ledsChanged)
    Q_PROPERTY(QVariantMap colours MEMBER m_colours NOTIFY ledsChanged)
    Q_PROPERTY(QString boardImage READ getBoardImage NOTIFY boardImageChanged)
    Q_PROPERTY(bool hasDFU MEMBER m_hasDFU NOTIFY dfuFound)
    Q_PROPERTY(bool isOpen READ getOpen NOTIFY portStateChanged)
    Q_PROPERTY(ArdwiinoDefines::InputType inputType MEMBER m_input_type NOTIFY inputTypeChanged)
    Q_PROPERTY(ArdwiinoDefines::SubType type MEMBER m_type NOTIFY typeChanged)
    Q_PROPERTY(ArdwiinoDefines::GyroOrientation orientation MEMBER m_orientation NOTIFY orientationChanged)
    Q_PROPERTY(ArdwiinoDefines::TiltType tiltType MEMBER m_tilt NOTIFY tiltTypeChanged)
    Q_PROPERTY(ArdwiinoDefines::FretLedMode ledType MEMBER m_led NOTIFY ledTypeChanged)
    Q_PROPERTY(int sensitivity MEMBER m_sensitivity NOTIFY tiltSensitivityChanged)
    Q_PROPERTY(int triggerThreshold MEMBER m_trigger_threshold NOTIFY triggerThresholdChanged)
    Q_PROPERTY(int joyThreshold MEMBER m_joy_threshold NOTIFY joyThresholdChanged)
    Q_PROPERTY(bool mapJoystick MEMBER m_map_joy NOTIFY mapJoystickChanged)
    Q_PROPERTY(bool mapStartSelectHome MEMBER m_map_start_sel_home NOTIFY mapStartSelectHomeChanged)
public:
    explicit Port(const QSerialPortInfo &serialPortInfo, QObject *parent = nullptr);
    explicit Port(QObject *parent = nullptr);
    explicit Port(board_t board, QObject *parent = nullptr);
    board_t getBoard() const {
        return m_board;
    }
    void prepareRescan();
    void prepareUpload();
    void open(const QSerialPortInfo &serialPortInfo);
    void close();
    void jumpUNO();
    void jump();
    void handleConnection(const QSerialPortInfo& info);
    void handleDisconnection(const QSerialPortInfo& info);
    void write(QByteArray id);
    void writeNoResp(QByteArray id);
signals:
    void hasAutoBindChanged();
    void descriptionChanged();
    void pinsChanged();
    void pinInvertsChanged();
    void boardImageChanged();
    void inputTypeChanged();
    void typeChanged();
    void ledTypeChanged();
    void orientationChanged();
    void tiltTypeChanged();
    void tiltSensitivityChanged();
    void dfuFound();
    void portStateChanged();
    void readyChanged();
    void outdatedChanged();
    void isArdwiinoChanged();
    void triggerThresholdChanged();
    void joyThresholdChanged();
    void mapJoystickChanged();
    void mapStartSelectHomeChanged();
    void detectedPinChanged();
    void ledsChanged();
    void midiChanged();
    void disconnectedChanged();
    void validChanged();

public slots:
    void readyRead();
    void writeConfig();
    void scanAfterDFU();
    void touch();
    bool isAlreadyDFU() const {
        return m_isAlreadyDFU;
    }
    bool isReady() const {
        return m_isReady;
    }
    bool isValid() const {
        return m_isValid;
    }
    bool isDisconnected() const {
        return m_disconnected;
    }
    QString description() const {
        return m_description;
    }
    bool getOpen() const {
        return m_serialPort != nullptr && m_serialPort->isOpen();
    }
    board_t board() const {
        return m_board;
    }
    QString boardShortName() const {
        return m_board.shortName;
    }
    QString boardName() const {
        return m_board.name;
    }
    void setBoard(QString boardName, uint freq) {
        m_board = ArdwiinoLookup::getInstance()->findByBoard(boardName);
        if (freq != 0) {
            m_board.cpuFrequency = freq;
        }
    }
    bool isArdwiino() const {
        return m_isArdwiino;
    }
    bool isOutdated() const {
        return m_isOutdated;
    }
    bool isGuitar() {
        return ArdwiinoDefines::getName(m_type).toLower().contains("guitar");
    }
    bool isDrum() {
        return ArdwiinoDefines::getName(m_type).toLower().contains("drum");
    }
    bool isMIDI() {
        return ArdwiinoDefines::getName(m_type).toLower().contains("midi");
    }
    bool isKeyboard() {
        return m_type >= ArdwiinoDefines::KEYBOARD_GAMEPAD && m_type <= ArdwiinoDefines::KEYBOARD_ROCK_BAND_DRUMS;
    }
    bool isMouse() {
        return m_type == ArdwiinoDefines::MOUSE;
    }
    bool isLiveGuitar() {
        return m_type == ArdwiinoDefines::XINPUT_LIVE_GUITAR;
    }
    bool isWii() {
        return m_input_type == ArdwiinoDefines::WII;
    }
    bool isRB() {
        return m_type == ArdwiinoDefines::WII_ROCK_BAND_GUITAR || m_type == ArdwiinoDefines::PS3_ROCK_BAND_GUITAR || m_type == ArdwiinoDefines::XINPUT_ROCK_BAND_GUITAR;
    }
    bool hasAddressableLEDs() {
        return m_led == ArdwiinoDefines::APA102;
    }
    QString getPort() const {
        return m_port;
    }
    bool ready() const {
        return m_isReady;
    }
    QVariantList getLEDs() const {
        return m_leds;
    }
    QVariantMap getColours() const {
        return m_colours;
    }
    QString getBoardImage() const {
        return m_board.image;
    }
    void findDigital(QJSValue callback);
    void findAnalog(QJSValue callback);


    void moveLED(int from, int to) {
        m_leds.move(from,to);
        ledsChanged();
    }
    uint8_t getTriggerThreshold() {
        return m_trigger_threshold;
    }
    uint8_t getJoyThreshold() {
        return m_joy_threshold;
    }
    bool getMapJoystick() {
        return m_map_joy;
    }
    bool getMapStartSelectHome() {
        return m_map_start_sel_home;
    }
    void handleError(QSerialPort::SerialPortError serialPortError);
    void readDescription();
    void prepareUpdate();
    void readAllData();
    int getTilt() {
        //TODO: We currently do not expose this from the serial api, and probably should.
//        QByteArray a;
//        read(, a, &m_controller, sizeof(controller_t));
//        READ_CONFIG(CONFIG)
//        return (int((m_controller.r_y * 360.0) / 65535.0) % 360)/2;
        return 0;
    }
    int getSensitivity() {
        return m_sensitivity;
    }
private:
    void pushWrite(QByteArray id);
    QQueue<QByteArray> m_dataToWrite;
    void loadPins();
    void savePins();
    void loadKeys();
    void saveKeys();
    void saveLEDs();
    void loadLEDs();
    void loadMIDI();
    void saveMIDI();
    void readData();
    void updateControllerName();
    QByteArray read(QByteArray);
    void rescan(const QSerialPortInfo &serialPortInfo);
    QString m_description;
    QString m_port;
    QSerialPort* m_serialPort;
    board_t m_board;
    bool m_isArdwiino;
    bool m_isOldAPIArdwiino;
    bool m_isOutdated;
    bool m_hasDFU;
    bool m_isReady;
    bool m_disconnected;
    bool m_hasPinDetectionCallback;
    bool m_isAlreadyDFU;
    QJSValue m_pinDetectionCallback;
    QVariantMap m_keys;
    QVariantMap m_pins;
    QVariantList m_leds;
    QVariantMap m_colours;
    QVariantMap m_pin_inverts;
    QVariantMap m_midi_type;
    QVariantMap m_midi_note;
    QVariantMap m_midi_channel;
    Controller_t m_controller{};
    bool readyForRead;
    ArdwiinoDefines::SubType m_type = ArdwiinoDefines::XINPUT_GAMEPAD;
    ArdwiinoDefines::GyroOrientation m_orientation;
    ArdwiinoDefines::TiltType m_tilt;
    ArdwiinoDefines::FretLedMode m_led;
    ArdwiinoDefines::InputType m_input_type;
    uint8_t m_trigger_threshold;
    uint8_t m_joy_threshold;
    bool m_isValid;
    bool m_map_joy;
    bool m_map_start_sel_home;
    int m_sensitivity;

    uint8_t read_8(QByteArray a);
    uint16_t read_16(QByteArray a);
    uint32_t read_32(QByteArray a);
    QList<uint8_t> read_8_n(QByteArray a, uint8_t count);
    QByteArray data_slot(uint8_t slot);
    QByteArray data_data(uint8_t slot, uint8_t data);
    QByteArray data_extra(uint8_t slot, uint8_t data, uint8_t extra);
    QByteArray data_extra_pins(uint8_t slot, uint8_t data, uint8_t pin, uint8_t extra);
    QByteArray data_extra_pins_32(uint8_t slot, uint8_t data, uint8_t pin, uint32_t extra);
};

#endif // PORT_H
