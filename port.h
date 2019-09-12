#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "ardwiinolookup.h"
#include "submodules/Ardwiino/src/shared/config/config.h"
#include "submodules/Ardwiino/src/shared/controller/controller.h"

class Port : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(bool isArdwiino READ isArdwiino)
    Q_PROPERTY(QString image READ getImage NOTIFY imageChanged)
    Q_PROPERTY(bool hasDFU MEMBER m_hasDFU NOTIFY dfuFound)
public:
    explicit Port(const QSerialPortInfo &serialPortInfo, QObject *parent = nullptr);
    explicit Port(QObject *parent = nullptr);
    board_t getBoard() const {
        return m_board;
    }
    void prepareUpload();
    void findNew();
    void open(const QSerialPortInfo &serialPortInfo);
    void close();
signals:
    void descriptionChanged(QString newValue);
    void imageChanged(QString newValue);
    void dfuFound(bool found);

public slots:
    void writeConfig();
    QString description() const {
        return m_description;
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
    QString getPort() const {
        return m_port;
    }
    QString getImage() const {
        return Controllers::getImage(Controllers::Value(m_config.sub_type));
    }
    Controllers::Value getType() const {
        return Controllers::Value(m_config.sub_type);
    }
    MPU6050Orientations::Value getOrientation() const {
        return MPU6050Orientations::Value(m_config.mpu_6050_orientation);
    }
    InputTypes::Value getInputType() const {
        return InputTypes::Value(m_config.input_type);
    }
    TiltTypes::Value getTiltType() const {
        return TiltTypes::Value(m_config.tilt_type);
    }
    void setType(Controllers::Value value) {
        m_config.sub_type = value;
        imageChanged(getImage());
    }
    void setInputType(InputTypes::Value value) {
        m_config.input_type = value;
    }
    void setTiltType(TiltTypes::Value value) {
        m_config.tilt_type = value;
    }
    void setOrientation(MPU6050Orientations::Value value) {
        m_config.mpu_6050_orientation = value;
    }
    void handleError(QSerialPort::SerialPortError serialPortError);
    bool findNewAsync();
    void readDescription();
private:
    void readData();
    void read(char id, QByteArray& location, unsigned long size);
    void rescan(const QSerialPortInfo &serialPortInfo);
    QString m_description;
    QString m_port;
    QSerialPort* m_serialPort;
    board_t m_board;
    QList<QSerialPortInfo> m_port_list;
    bool m_isArdwiino;
    bool m_hasDFU;
    config_t m_config_device;
    config_t m_config;
};

#endif // PORT_H
