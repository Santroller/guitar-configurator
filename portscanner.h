#ifndef PORTSCANNER_H
#define PORTSCANNER_H

#include <QObject>
#include <QSerialPortInfo>
#include <QList>
#include <QImage>
#include "port.h"
#include "programmer.h"

class PortScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> model READ model NOTIFY modelChanged)
    Q_PROPERTY(Port* selected MEMBER m_selected NOTIFY selectedChanged)
public:
    explicit PortScanner(Programmer* programmer, QObject *parent = nullptr);
    Port* selectedPort() const {
        return m_selected;
    };
signals:
    void modelChanged();
    void selectedChanged(Port* newValue);
public slots:
    void update();
    void addPort(const QSerialPortInfo& port);
    void removePort(const QSerialPortInfo& port);
    void fixLinux();
    QStringList getImages(QString base);
    QString findElement(QString base, int width, int heither, int mouseX, int mouseY);
    QList<QObject*> model() const {
        return m_model;
    }

    inline QString getOSString() {
#ifdef Q_OS_LINUX
        return "<br />If you are not able to detect any devices, you may need to add yourself to the uucp and dialout groups. <br /> For information, <a href=\"https://www.arduino.cc/en/guide/linux#toc6\">click here</a>.<br/>Is your gamepad not detected in game? Click the button below to fix the problem. <br/> Note that this fix will need to be applied after every reboot.";
#else
        return "";
#endif
    }
    inline bool isLinux() {
#ifdef Q_OS_LINUX
        return true;
#endif
        return false;
    }
private:
    void clearImages();
    QList<QObject*> m_model;
    Port* m_selected;
    Programmer* programmer;
    QStringList images;
};

#endif // PORTSCANNER_H
