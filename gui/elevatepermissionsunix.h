#ifndef ELEVATEPERMISSIONSLINUX_H
#define ELEVATEPERMISSIONSLINUX_H
#include <QObject>
#include <KAuth>

class ElevatePermissionsUnix: public QObject
{
    Q_OBJECT
public:
    ElevatePermissionsUnix();
public slots:
    void helperPercent(KJob*, unsigned long step);
    void helperData(const QVariantMap & data);
    void finished(KJob* job);

};

#endif // ELEVATEPERMISSIONSLINUX_H
