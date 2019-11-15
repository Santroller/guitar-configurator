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
    void onReady();
    void program(Port* port);
    void complete(int exitCode, QProcess::ExitStatus exitStatus);
    QString getStatusDescription() {
        return QString("Programming Status: %1 (%2%)").arg(Status::toString(m_status)).arg(QString().sprintf("%.2f",m_process_percent*100));
    }
    void setRestoring(bool restore) {
        m_restore = restore;
        m_status = Status::WAIT;
        m_process_out = "";
        m_process_percent = 0;
        emit restoreChanged(restore);
        emit statusChanged(m_status);
        emit statusVChanged(getStatusDescription());
        emit processOutChanged(m_process_out);
        emit processPercentChanged(m_process_percent);
    }
private:
    void programDFU();
    void programAvrDude();
    QProcess* m_process;
    QString m_process_out;
    double m_process_percent;
    Status::Value m_status;
    Port* m_port;
    bool m_restore;
    board_t detectBoard();
};
#endif // PROGRAMMER_H
