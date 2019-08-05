#ifndef PORTSCANNER_H
#define PORTSCANNER_H

#include <QObject>
#include <QTimer>
#include <QSerialPortInfo>
#include <QList>
#include "port.h"

class PortScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> model READ model NOTIFY modelChanged)
public:
    explicit PortScanner(QObject *parent = nullptr);
signals:
    void modelChanged(QList<QObject*> newValue);
public slots:
    void checkPorts();
    QList<QObject*> model() const {
        return m_model;
    }
private:
    QList<QObject*> m_model;
};

#endif // PORTSCANNER_H
