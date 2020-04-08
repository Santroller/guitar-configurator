#ifndef CLONEHEROINFO_H
#define CLONEHEROINFO_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QFile>
#include <QSettings>
#include <QSet>
#include "portscanner.h"

#if defined Q_OS_MAC
    #include <mach/host_info.h>
    #include <mach/mach_host.h>
    #include <mach/shared_region.h>
    #include <mach/mach.h>
    #include <mach-o/dyld.h>
#endif

class LEDHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString gameFolder READ getGameFolder WRITE setGameFolder NOTIFY gameFolderChanged)
    Q_PROPERTY(QString version MEMBER m_version NOTIFY versionChanged)
public:
    explicit LEDHandler(QGuiApplication* application, PortScanner* scanner, QObject *parent = nullptr);
    QTimer *timer;
    void findVersion();
    QString getGameFolder() {
        return m_gameFolder;
    }
signals:
    void gameFolderChanged();
    void versionChanged();
public slots:
    void startGame();
private slots:
 void tick();
 void setGameFolder(QString string);

private:
 QSettings settings;
 QNetworkAccessManager m_WebCtrl;
 QString binary;
 Q_PID pid;
 QString m_version;
 QMap<QString, QString> hashes;
 QList<QString> hashedFiles;
 QProcess process;
 PortScanner* scanner;
 int lastScore = 0;
 int shownNote = 0;
 int countdown = 0;
 QString m_gameFolder;
 QByteArray lastData;
 qint64 base=0;
 qint64 readFromProc(quint64 , qint64 addr, qint64 *buf);
 qint64 readData(qint64 base, QList<qint64> &path, qint64 pathCount, qint64 *buf);

 QList<qint64> pointerPathBasePlayer;
 QList<qint64> pointerPathCurrentNote;

 QString platform;
 QString lib;
 // int32_t
 int offsetButtonsPressed;
 // bool
 int offsetStarPowerActivated;
 // bool
 int offsetIsStarPower;

 int offsetScore;
 // int32_t
 int offsetCurrentNote;
#if defined Q_OS_LINUX
 QFile* inputFile;
#endif
#if defined Q_OS_MAC
 mach_port_t task;
#endif

};

#endif // CLONEHEROINFO_H
