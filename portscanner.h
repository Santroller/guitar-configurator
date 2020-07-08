#ifndef PORTSCANNER_H
#define PORTSCANNER_H

#include <QImage>
#include <QList>
#include <QObject>
#include <QSerialPortInfo>
#include <QSettings>
#include <QTimer>

#include "devices/device.h"
#include "devices/outdated_ardwiino.h"
#include "programmer.h"

class PortScanner : public QObject {
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> model READ model NOTIFY modelChanged)
    Q_PROPERTY(Device* selected READ getSelected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(bool isGraphical READ isGraphical NOTIFY graphicalChanged)
    Q_PROPERTY(bool hasSelected MEMBER m_hasSelected NOTIFY hasSelectedChanged)
   public:
    explicit PortScanner(Programmer* programmer, QObject* parent = nullptr);
    int hotplug_callback(UsbDevice_t devt, libusb_hotplug_event event);
   signals:
    void graphicalChanged();
    void modelChanged();
    void selectedChanged();
    void hasSelectedChanged();
   public slots:
    void tick();
    bool isGraphical() const {
        return m_graphical;
    }
    void setSelected(Device* port);
    Device* getSelected() const {
        return m_selected;
    }
    void update();
    void serialDeviceDetected(const QSerialPortInfo& port);
    void serialDeviceUnplugged(const QSerialPortInfo& port);
    void fixLinux();
    void toggleGraphics();
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
    void scanDevices();
    void add(Device* device);
    void remove(Device* device);
    bool m_hasSelected;
    void clearImages();
    QList<QObject*> m_model;
    Device* m_selected;
    Programmer* programmer;
    QStringList images;
    bool m_graphical;
    QSettings settings;
    QList<QProcess*> m_processes;
    Device* m_emptyDevice;
    QList<UsbDevice_t> existingDevices;
    QTimer* timer;
    bool m_hasHotplug;
};

#endif  // PORTSCANNER_H
