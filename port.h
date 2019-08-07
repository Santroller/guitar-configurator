#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class Port : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(bool isArdwiino READ isArdwiino)
public:
    explicit Port(const QSerialPortInfo *serialPortInfo, QObject *parent = nullptr);
signals:
    void descriptionChanged(QString newValue);

public slots:
    QString description() const {
        return m_description;
    }
    bool isArdwiino() const {
        return m_isArdwiino;
    }
    QString getPort() const {
        return m_port;
    }
    void handleError(QSerialPort::SerialPortError serialPortError);
    void update();
private:
    QString m_description;
    QString m_port;
    QSerialPort* m_serialPort;
    QByteArray readData;
    bool m_isArdwiino;
};

#endif // PORT_H
