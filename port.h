#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "ardwiinolookup.h"
#include "submodules/Ardwiino/src/shared/output/usb/API.h"
#include <math.h>
#define READ_INFO(slot) QByteArray(1, COMMAND_READ_INFO) + QByteArray(1,slot)
#define READ_CONFIG(slot) QByteArray(1, COMMAND_READ_CONFIG_VALUE) + QByteArray(1,slot)
#define WRITE_CONFIG(slot, value) QByteArray(1, COMMAND_WRITE_CONFIG_VALUE) + QByteArray(1, slot) + QByteArray(1, value)
#define WRITE_CONFIG_PINS(slot, neg,pos) QByteArray(1, COMMAND_WRITE_CONFIG_VALUE) + QByteArray(1, slot) + QByteArray(1, neg) + QByteArray(1, pos)
class Port : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(bool isArdwiino READ isArdwiino NOTIFY isArdwiinoChanged)
    Q_PROPERTY(bool isOutdated READ isOutdated NOTIFY outdatedChanged)
    Q_PROPERTY(bool isGuitar READ isGuitar NOTIFY typeChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)
    Q_PROPERTY(QVariantMap pin_inverts MEMBER m_pin_inverts NOTIFY pinInvertsChanged)
    Q_PROPERTY(QVariantMap pins MEMBER m_pins NOTIFY pinsChanged)
    Q_PROPERTY(QString image READ getImage NOTIFY imageChanged)
    Q_PROPERTY(QString boardImage READ getBoardImage NOTIFY boardImageChanged)
    Q_PROPERTY(bool hasDFU MEMBER m_hasDFU NOTIFY dfuFound)
    Q_PROPERTY(bool isOpen READ getOpen NOTIFY portStateChanged)
    Q_PROPERTY(InputTypes::Value inputType READ getInputType WRITE setInputType NOTIFY inputTypeChanged)
    Q_PROPERTY(Controllers::Value type READ getType WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(MPU6050Orientations::Value orientation READ getOrientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(TiltTypes::Value tiltType READ getTiltType WRITE setTiltType NOTIFY tiltTypeChanged)
    Q_PROPERTY(int sensitivity READ getSensitivity WRITE setSensitivity NOTIFY tiltSensitivityChanged)
public:
    explicit Port(const QSerialPortInfo &serialPortInfo, QObject *parent = nullptr);
    explicit Port(QObject *parent = nullptr);
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
signals:
    void descriptionChanged();
    void imageChanged();
    void pinsChanged();
    void pinInvertsChanged();
    void boardImageChanged();
    void inputTypeChanged();
    void typeChanged();
    void orientationChanged();
    void tiltTypeChanged();
    void tiltSensitivityChanged();
    void dfuFound();
    void portStateChanged();
    void readyChanged();
    void outdatedChanged();
    void isArdwiinoChanged();

public slots:
    void writeConfig();
    bool isReady() const {
        return m_isReady;
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
        m_board.cpuFrequency = freq;
    }
    bool isArdwiino() const {
        return m_isArdwiino;
    }
    bool isOutdated() const {
        return m_isOutdated;
    }
    bool isGuitar() {
        return ArdwiinoLookup::getInstance()->getControllerTypeName(getType()).toLower().contains("guitar");
    }
    QString getPort() const {
        return m_port;
    }
    bool ready() const {
        return m_isReady;
    }
    QString getImage() {
        if (readyForRead && m_serialPort) return Controllers::getImage(getType());
        return Controllers::getImage(Controllers::XINPUT_GAMEPAD);
    }
    QString getBoardImage() const {
        return m_board.image;
    }
    Controllers::Value getType() {
        return Controllers::Value(read_single(READ_CONFIG(CONFIG_SUB_TYPE)));
    }
    MPU6050Orientations::Value getOrientation() {
        return MPU6050Orientations::Value(read_single(READ_CONFIG(CONFIG_MPU_6050_ORIENTATION)));
    }
    InputTypes::Value getInputType() {
        return InputTypes::Value(read_single(READ_CONFIG(CONFIG_INPUT_TYPE)));
    }
    TiltTypes::Value getTiltType() {
        return TiltTypes::Value(read_single(READ_CONFIG(CONFIG_TILT_TYPE)));
    }
    void setType(Controllers::Value value) {
        write(WRITE_CONFIG(CONFIG_SUB_TYPE, value));
        imageChanged();
        typeChanged();
    }
    void setInputType(InputTypes::Value value) {
        write(WRITE_CONFIG(CONFIG_INPUT_TYPE, value));
        inputTypeChanged();

    }
    void setTiltType(TiltTypes::Value value) {
        write(WRITE_CONFIG(CONFIG_TILT_TYPE, value));
        tiltTypeChanged();
    }
    void setOrientation(MPU6050Orientations::Value value) {
        write(WRITE_CONFIG(CONFIG_MPU_6050_ORIENTATION, value));
        orientationChanged();
    }
    void handleError(QSerialPort::SerialPortError serialPortError);
    void startConfiguring();
    void readDescription();
    void loadPins();
    void savePins();
    void loadKeys();
    void saveKeys();
    int getTilt() {
        //TODO: We currently do not expose this from the serial api, and probably should.
//        QByteArray a;
//        read(, a, &m_controller, sizeof(controller_t));
//        READ_CONFIG(CONFIG)
//        return (int((m_controller.r_y * 360.0) / 65535.0) % 360)/2;
        return 0;
    }
    int getSensitivity() {
        return read_single(READ_CONFIG(CONFIG_TILT_SENSITIVITY));
    }
    void setSensitivity(int s) {
        write(WRITE_CONFIG(CONFIG_TILT_SENSITIVITY, s));
        tiltSensitivityChanged();
    }
private:
    void readData();
    void updateControllerName();
    uint8_t read_single(QByteArray id);
    QByteArray read(QByteArray);
    void write(QByteArray id);
    void rescan(const QSerialPortInfo &serialPortInfo);
    QString m_description;
    QString m_port;
    QSerialPort* m_serialPort;
    board_t m_board;
    bool m_isArdwiino;
    bool m_isOldArdwiino;
    bool m_isOutdated;
    bool m_hasDFU;
    bool m_isReady;
    QVariantMap m_pins;
    QVariantMap m_pin_inverts;
    controller_t m_controller{};
    bool readyForRead;
};

#endif // PORT_H
