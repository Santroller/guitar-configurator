#pragma once
#include <math.h>

#include <QDebug>
#include <QJSValue>
#include <QObject>
#include <QQueue>
#include <QSettings>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "ardwiinolookup.h"
#include "device.h"
#include "deviceconfiguration.h"
#include "submodules/Ardwiino/src/shared/output/serial_commands.h"
#include "usb/usbdevice.h"
class Ardwiino : public Device {
    Q_OBJECT
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)
    Q_PROPERTY(DeviceConfiguration* config MEMBER m_configuration NOTIFY configurationChanged)
    Q_PROPERTY(DeviceConfiguration* defaultConfig MEMBER m_default_configuration NOTIFY configurationChanged)
    Q_PROPERTY(QStringList presets READ getPresets NOTIFY presetsChanged)
   public:
    explicit Ardwiino(UsbDevice_t devt, QObject* parent = nullptr);
    QString getDescription();
    bool isReady();
    virtual void close();
    virtual bool open();
    virtual void bootloader();
    virtual bool isConfigurable() {
        return m_configurable;
    }
    board_t board() const {
        return m_board;
    }
    QString boardShortName() const {
        return m_board.shortName;
    }
    QString boardName() const {
        return m_board.name;
    }
    bool isOutdated() const {
        return m_isOutdated;
    }
    DeviceConfiguration* getConfig() const {
        return m_configuration;
    }
    int getRFID() const {
        return m_rfID;
    }
    void writeChunked(uint8_t id, QByteArray data);
    qint32 generateClientRFID();
   public slots:
    void writeConfig();
    void resetConfig();
    void findDigital(QJSValue callback);
    void findAnalog(QJSValue callback);
    int readAnalog(int pin);
    void cancelFind();
    void startFind();
    // When using rf, we need a way to get the board for the rf transmitter, not the receiver
    inline QString getDirectBoardImage() const {
        if (m_configuration->getRfRfInEnabled()) {
            return m_board_rf.image;
        }
        return m_board.image;
    }
    void savePreset(QString name, QString config);
    QStringList getPresets();
    QString getPreset(QString name);
    void removePreset(QString name);
    void importPreset(QString file);
    void exportPreset(QString name, QString file);
   signals:
    void readyChanged();
    void ledsChanged();
    void midiChanged();
    void configurationChanged();
    void presetsChanged();

   protected:
    QString m_processor;

   private:
    // data_t readData();
    QByteArray readConfig();
    UsbDevice m_usbDevice;
    DeviceConfiguration* m_configuration;
    DeviceConfiguration* m_default_configuration;
    bool m_isOpen;
    bool m_isOutdated;
    bool m_hasPinDetectionCallback;
    bool m_configurable;
    QJSValue m_pinDetectionCallback;
    uint16_t m_extension;
    uint32_t m_rfID;
    board_t m_board_rf;
    QSettings settings;
    inline virtual bool isEqual(const Device& other) const {
        return m_deviceID.bus == other.getUSBDevice().bus && m_deviceID.port == other.getUSBDevice().port;
    }
};
