#ifndef PORTSCANNER_H
#define PORTSCANNER_H

#include <QObject>
#include <QSerialPortInfo>
#include <QList>
#include <QImage>
#include "port.h"
#include "programmer.h"
#include <QSettings>

class PortScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> model READ model NOTIFY modelChanged)
    Q_PROPERTY(Port* selected READ getSelected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(bool isGraphical READ isGraphical NOTIFY graphicalChanged)
    Q_PROPERTY(bool hasSelected MEMBER m_hasSelected NOTIFY hasSelectedChanged)
public:
    explicit PortScanner(Programmer* programmer, QObject *parent = nullptr);
    Port* selectedPort() const {
        return m_selected;
    };
signals:
    void graphicalChanged();
    void modelChanged();
    void selectedChanged();
    void hasSelectedChanged();
public slots:
    bool isGraphical() const {
        return m_graphical;
    }
    void setSelected(Port* port);
    Port* getSelected() const {
        return m_selected;
    }
    void update();
    void addPort(const QSerialPortInfo& port);
    void removePort(const QSerialPortInfo& port);
    void fixLinux();
    void toggleGraphics();
    QString findElement(QString base, int width, int heither, int mouseX, int mouseY);
    void complete(int exitCode, QProcess::ExitStatus exitStatus);
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
    bool m_hasSelected;
    void clearImages();
    QList<QObject*> m_model;
    Port* m_selected;
    Programmer* programmer;
    QStringList images;
    bool m_graphical;
    QSettings settings;
    QProcess* m_process[sizeof(ArdwiinoLookup::boards) / sizeof(ArdwiinoLookup::boards[0])];
};

#endif // PORTSCANNER_H
