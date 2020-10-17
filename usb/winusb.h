#ifndef WINUSB_H
#define WINUSB_H

#include <QObject>

class WinUsb : public QObject
{
    Q_OBJECT
public:
    explicit WinUsb(QObject *parent = nullptr);

signals:

};

#endif // WINUSB_H
