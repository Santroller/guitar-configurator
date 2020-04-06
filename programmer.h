#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include <QObject>
#include <QProcess>
#include <QDebug>
#include "port.h"
#include "status.h"

class Programmer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString process_out MEMBER m_process_out NOTIFY processOutChanged)
    Q_PROPERTY(double process_percent MEMBER m_process_percent NOTIFY processPercentChanged)
    Q_PROPERTY(Status::Value status MEMBER m_status NOTIFY statusChanged)
    Q_PROPERTY(QString statusDescription READ getStatusDescription NOTIFY statusVChanged)
    Q_PROPERTY(bool restore MEMBER m_restore NOTIFY restoreChanged)
public:
    explicit Programmer(QObject *parent = nullptr);
signals:
    void processOutChanged(QString newValue);
    void processPercentChanged(double newValue);
    void restoreChanged(bool newValue);
    void statusChanged(Status::Value newValue);
    void statusVChanged(QString newValue);
public slots:
    bool program(Port* port);
    void onReady();
    void complete(int exitCode, QProcess::ExitStatus exitStatus);
    QString getStatusDescription() {
        return QString("Programming Status: %1 (%2%)").arg(Status::toString(m_status)).arg(QString::number(m_process_percent*100,'f',2));
    }
    bool getRestore() {
        return m_restore;
    }
    void setRestoring(bool restore) {
        m_restore = restore;
        m_process_out = "";
        m_process_percent = 0;
        m_status = Status::NOT_PROGRAMMING;
        emit restoreChanged(restore);
        emit statusChanged(m_status);
        emit statusVChanged(getStatusDescription());
        emit processOutChanged(m_process_out);
        emit processPercentChanged(m_process_percent);
    }
    void startProgramming() {
        m_status = Status::WAIT;
        emit statusChanged(m_status);
        emit statusVChanged(getStatusDescription());
    }
private:
    void programDFU();
    void programAvrDude();
    QProcess* m_process{};
    QString m_process_out;
    double m_process_percent{};
    Status::Value m_status;
    Port* m_port;
    bool m_restore;
    board_t detectBoard();
};
#endif // PROGRAMMER_H
