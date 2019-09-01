#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include <QObject>
#include <QProcess>

class Programmer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString process_out MEMBER m_process_out NOTIFY processOutChanged)
public:
    explicit Programmer(QObject *parent = nullptr);
signals:
    void processOutChanged(QString newValue);
public slots:
    void onReady();
private:
    QProcess* m_process;
    QString m_process_out;
};
#endif // PROGRAMMER_H
