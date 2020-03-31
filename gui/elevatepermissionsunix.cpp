#include "elevatepermissionsunix.h"
#include <QDebug>
ElevatePermissionsUnix::ElevatePermissionsUnix()
{

    QString filename = "/proc/meminfo";
    QVariantMap args;
    args["filename"] = filename;
    KAuth::Action longAction(QLatin1String("org.kde.kf5auth.example.longaction"));
    longAction.setHelperId("org.kde.kf5auth.example");
    KAuth::ExecuteJob *job = longAction.execute();
    connect(job, SIGNAL(percent(KJob*, unsigned long)), this, SLOT(helperPercent(KJob*, unsigned long)));
    connect(job, SIGNAL(newData(const QVariantMap &)), this, SLOT(helperData(const QVariantMap &)));
    connect(job, SIGNAL(result(KJob*)), this, SLOT(finished(KJob*)));
    job->start();
}
void ElevatePermissionsUnix::helperPercent(KJob*, unsigned long step)
 {
     qDebug() << step;
 }

 void ElevatePermissionsUnix::helperData(const QVariantMap & data)
 {
     qDebug() << data;
     //...
 }

 void ElevatePermissionsUnix::finished(KJob* job)
 {
     qDebug() << "helper finished " << job->errorText();
 }
