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
    void addPort(const QSerialPortInfo& port);
    void removePort(const QSerialPortInfo& port);
    QList<QObject*> model() const {
        return m_model;
    }

    inline QString getOSString() {
#ifdef Q_OS_LINUX
        return "<br />If you are not able to detect any devices, you may need to add yourself to the uucp and dialout groups. <br /> For information, <a href=\"https://www.arduino.cc/en/guide/linux#toc6\">click here</a>";
#else
        return "";
#endif
    }
private:
    QList<QObject*> m_model;
    Port* m_selected;
    Programmer* programmer;
};

#endif // PORTSCANNER_H
