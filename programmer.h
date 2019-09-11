#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include <QObject>
#include <QProcess>
#include "port.h"
#include "status.h"

class Programmer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString process_out MEMBER m_process_out NOTIFY processOutChanged)
    Q_PROPERTY(double process_percent MEMBER m_process_percent NOTIFY processPercentChanged)
    Q_PROPERTY(Status::Value status MEMBER m_status NOTIFY statusChanged)
public:
    explicit Programmer(QObject *parent = nullptr);
signals:
    void processOutChanged(QString newValue);
    void processPercentChanged(double newValue);
    void statusChanged(Status::Value newValue);
public slots:
    void onReady();
    void program(Port* port);
    void complete(int exitCode, QProcess::ExitStatus exitStatus);
private:
    void programDFU();
    void programAvrDude();
    QProcess* m_process;
    QString m_process_out;
    double m_process_percent;
    Status::Value m_status;
    Port* m_port;
};
#endif // PROGRAMMER_H
