#ifndef UNIXSERIALHOTPLUG_H
#define UNIXSERIALHOTPLUG_H

#include <QObject>
#include <QTimer>
#include <QSerialPortInfo>
#include <QList>
#include "portscanner.h"
#include "programmer.h"

class UnixSerialHotplug : public QObject
{
    Q_OBJECT
public:
    explicit UnixSerialHotplug(PortScanner* scanner, QObject *parent = nullptr);
    QTimer *timer;
    QList<QSerialPortInfo> m_port_list;
    PortScanner* scanner;

signals:

public slots:
    void tick();
};

#endif // UNIXSERIALHOTPLUG_H
