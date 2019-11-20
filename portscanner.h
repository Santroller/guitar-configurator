#ifndef PORTSCANNER_H
#define PORTSCANNER_H

#include <QObject>
#include <QSerialPortInfo>
#include <QList>
#include "port.h"
#include "programmer.h"

class PortScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> model READ model NOTIFY modelChanged)
    Q_PROPERTY(Port* selected MEMBER m_selected NOTIFY selectedChanged)
public:
    explicit PortScanner(Programmer* programmer, QObject *parent = nullptr);
signals:
    void modelChanged(QList<QObject*> newValue);
    void selectedChanged(Port* newValue);
public slots:
    void addPort(QSerialPortInfo port);
    void removePort(QSerialPortInfo port);
    QList<QObject*> model() const {
        return m_model;
    }
private:
    QList<QObject*> m_model;
    Port* m_selected;
    Programmer* programmer;
};

#endif // PORTSCANNER_H
