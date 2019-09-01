#ifndef PORTSCANNER_H
#define PORTSCANNER_H

#include <QObject>
#include <QSerialPortInfo>
#include <QList>
#include "port.h"

class PortScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> model READ model NOTIFY modelChanged)
    Q_PROPERTY(Port* selected MEMBER m_selected NOTIFY selectedChanged)
public:
    explicit PortScanner(QObject *parent = nullptr);
signals:
    void modelChanged(QList<QObject*> newValue);
    void selectedChanged(Port* newValue);
public slots:
    void checkPorts();
    QList<QObject*> model() const {
        return m_model;
    }
private:
    QList<QObject*> m_model;
    Port* m_selected;
};

#endif // PORTSCANNER_H
