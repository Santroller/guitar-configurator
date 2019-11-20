#ifndef WINEVENTFILTER_H
#define WINEVENTFILTER_H
#include <QWindow>
#include <Windows.h>
#include <Dbt.h>
#include <QList>
#include <QAbstractNativeEventFilter>
#include "portscanner.h"
#include "programmer.h"
class WinSerialHotplug : public QAbstractNativeEventFilter
{
public:
    explicit WinSerialHotplug(PortScanner* scanner);
    void init(WId wid);
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
    PortScanner* scanner;

signals:

public slots:
    void tick();
private:
    QList<QSerialPortInfo> m_port_list;
};

#endif // WINEVENTFILTER_H
