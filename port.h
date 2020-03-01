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
class Port : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(bool isArdwiino READ isArdwiino)
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
    void handleConnection(const QSerialPortInfo& info);
signals:
    void descriptionChanged(QString newValue);
    void imageChanged(QString newValue);
    void pinsChanged(QVariantMap newValue);
    void pinInvertsChanged(QVariantMap newValue);
    void boardImageChanged(QString newValue);
    void inputTypeChanged(InputTypes::Value newValue);
    void typeChanged(Controllers::Value newValue);
    void orientationChanged(MPU6050Orientations::Value newValue);
    void tiltTypeChanged(TiltTypes::Value newValue);
    void tiltSensitivityChanged(int newValue);
    void dfuFound(bool found);
    void portStateChanged(bool open);

public slots:
    void writeConfig();
    QString description() const {
        return m_description;
    }
    bool getOpen() const {
        return m_serialPort != nullptr && m_serialPort->isOpen();
    }
    board_t board() const {
        return m_board;
    }
    QString boardName() const {
        return m_board.name;
    }
    void setBoardFreq(uint freq) {
        m_board.cpuFrequency = freq;
    }
    bool isArdwiino() const {
        return m_isArdwiino;
    }
    bool isGuitar() const {
        return ArdwiinoLookup::getInstance()->getControllerTypeName(getType()).toLower().contains("guitar");
    }
    QString getPort() const {
        return m_port;
    }
    QString getImage() const {
        return Controllers::getImage(Controllers::XINPUT_GUITAR);
//        return Controllers::getImage(Controllers::Value(m_config.main.sub_type));
    }
    QString getBoardImage() const {
//        return m_board.image;
        return "images/uno.png";
    }
    Controllers::Value getType() const {
        return Controllers::KEYBOARD;
//        return Controllers::Value(m_config.main.sub_type);
    }
    MPU6050Orientations::Value getOrientation() const {
        return MPU6050Orientations::NEGATIVE_X_TYPE;
//        return MPU6050Orientations::Value(m_config.axis.mpu_6050_orientation);
    }
    InputTypes::Value getInputType() const {
        return InputTypes::WII_TYPE;
//        return InputTypes::Value(m_config.main.input_type);
    }
    TiltTypes::Value getTiltType() const {
        return TiltTypes::NONE_SENSOR;
//        return TiltTypes::Value(m_config.main.tilt_type);
    }
    void setType(Controllers::Value value) {
//        m_config.main.sub_type = value;
        imageChanged(getImage());
        typeChanged(value);
    }
    void setInputType(InputTypes::Value value) {
//        m_config.main.input_type = value;
        inputTypeChanged(value);

    }
    void setTiltType(TiltTypes::Value value) {
//        m_config.main.tilt_type = value;
        tiltTypeChanged(value);
    }
    void setOrientation(MPU6050Orientations::Value value) {
//        m_config.axis.mpu_6050_orientation = value;
        orientationChanged(value);
    }
    void handleError(QSerialPort::SerialPortError serialPortError);
    void readDescription();
    void loadPins();
    void savePins();
    void loadKeys();
    void saveKeys();
    int getTilt() {
//        QByteArray a;
//        read(, a, &m_controller, sizeof(controller_t));
//        READ_CONFIG(CONFIG)
//        return (int((m_controller.r_y * 360.0) / 65535.0) % 360)/2;
        return 0;
    }
    int getSensitivity() const {
        return 0;
//        return m_config.axis.tilt_sensitivity;
    }
    void setSensitivity(int s) {
//        m_config.axis.tilt_sensitivity = int16_t(s);
        tiltSensitivityChanged(s);
    }
private:
    void readData();
    void updateControllerName();
    char read_single(QByteArray id);
    QByteArray read(QByteArray);
    void write(char id, void* dest, unsigned long size);
    void rescan(const QSerialPortInfo &serialPortInfo);
    QString m_description;
    QString m_port;
    QSerialPort* m_serialPort{};
    board_t m_board;
    bool m_isArdwiino{};
    bool m_hasDFU{};
    QVariantMap m_pins;
    QVariantMap m_pin_inverts;
    controller_t m_controller{};
};

#endif // PORT_H
