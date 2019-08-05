#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QSerialPortInfo>

class Port : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
public:
    explicit Port(const QSerialPortInfo *serialPortInfo, QObject *parent = nullptr);
signals:
    void descriptionChanged(QString newValue);

public slots:
    QString description() const {
        return m_description;
    }
private:
    QString m_description;
    const QSerialPortInfo *m_serialPort;
    bool isArdwiino();
};

#endif // PORT_H
