#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include <QDebug>
#include <QObject>
#include <QProcess>
#include <QTemporaryFile>

#include "devices/arduino.h"
#include "devices/ardwiino.h"
#include "devices/device.h"
#include "devices/dfu_arduino.h"
#include "devices/picoboot_device.h"
#include "status.h"

class Programmer : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString process_out MEMBER m_process_out NOTIFY processOutChanged)
    Q_PROPERTY(double process_percent MEMBER m_process_percent NOTIFY processPercentChanged)
    Q_PROPERTY(Status::Value status MEMBER m_status NOTIFY statusChanged)
    Q_PROPERTY(QString statusDescription READ getStatusDescription NOTIFY statusVChanged)
    Q_PROPERTY(bool restore MEMBER m_restore NOTIFY restoreChanged)
    Q_PROPERTY(bool rf MEMBER m_rf NOTIFY rfChanged)
   public:
    explicit Programmer(QObject* parent = nullptr);
   signals:
    void processOutChanged(QString newValue);
    void processPercentChanged(double newValue);
    void restoreChanged(bool newValue);
    void rfChanged(bool newValue);
    void statusChanged(Status::Value newValue);
    void statusVChanged(QString newValue);
   public slots:
    bool program(Device* port);
    QString programRF(QString hexFile);
    void prepareRF(Ardwiino* parent);
    void onReady();
    void complete(int exitCode, QProcess::ExitStatus exitStatus);
    QString getStatusDescription() {
        return QString("Programming Status: %1 (%2%)").arg(Status::toString(m_status)).arg(QString::number(m_process_percent, 'f', 2));
    }
    bool getRestore() {
        return m_restore;
    }
    Status::Value getStatus() {
        return m_status;
    }
    void setRestoring(bool restore) {
        m_restore = restore;
        m_process_out = "";
        setPercentage(0);
        m_status = Status::NOT_PROGRAMMING;
        emit restoreChanged(restore);
        emit statusChanged(m_status);
        emit processOutChanged(m_process_out);
    }
    void startProgramming() {
        m_status = Status::WAIT;
        emit statusChanged(m_status);
        setPercentage(0);
    }

   public:
    void deviceAdded(DfuArduino* device);
    void deviceAdded(Ardwiino* device);
    void deviceAdded(Arduino* device);
    void deviceAdded(PicobootDevice* device);
    void setPercentage(long dividend, long divisor, int step, int stepCount) {
        double perc = divisor ? ((100.0 * dividend) / divisor) : 100.0;
        setPercentage(((100.0 * step) / stepCount) + (perc / stepCount));
    }
    void setPercentage(double percentage) {
        m_process_percent = percentage;
        emit statusVChanged(getStatusDescription());
        emit processPercentChanged(m_process_percent);
    }

   private:
    void programDFU();
    void programAvrDude();
    void programPico();
    QProcess* m_process{};
    QString m_process_out;
    double m_process_percent{};
    Status::Value m_status;
    Device* m_device;
    bool m_restore;
    bool m_rf;
    bool m_isGeneric;
    int m_count;
    int m_step;
    int m_steps;
    Ardwiino* m_parent_device;
    QTemporaryFile* m_tmp_hex;
};
#endif  // PROGRAMMER_H
