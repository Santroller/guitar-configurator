#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include <QObject>
#include <QProcess>
#include "port.h"
enum AvrFileFormat {
    IntelHex='i',
    MotorolaSRecord='s',
    RawBinary='r',
    Elf='e',
    Immediate='m',
    AutoDetect='a',
    Decimal='d',
    Octal='o',
    Binary='b'
};

enum MemoryLocation {
    Flash,
    EEPROM
};


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
    void program(Port* port);
private:
    QProcess* m_process;
    QString m_process_out;
};
#endif // PROGRAMMER_H
