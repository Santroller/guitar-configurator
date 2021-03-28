#ifndef CLONEHEROINFO_H
#define CLONEHEROINFO_H

#include <QByteArray>
#include <QFile>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QProcess>
#include <QSet>
#include <QSettings>
#include <QTimer>

#include "portscanner.h"

#if defined Q_OS_MAC
#include <mach-o/dyld.h>
#include <mach/host_info.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/shared_region.h>
#endif

class LEDHandler : public QObject {
    typedef struct {
        QList<QString> files;
        QString binary;
        QString hash;
        QString version;
    } CloneHeroBundle;
    Q_OBJECT
    Q_PROPERTY(QString gameFolder READ getGameFolder WRITE setGameFolder NOTIFY gameFolderChanged)
    Q_PROPERTY(bool ready MEMBER m_ready NOTIFY readyChanged)
    Q_PROPERTY(QString version READ getVersion WRITE setVersion NOTIFY versionChanged)
    Q_PROPERTY(bool openEnabled READ getOpenEnabled WRITE setOpenEnabled NOTIFY openEnabledChanged)
    Q_PROPERTY(bool starPowerEnabled READ getStarPowerEnabled WRITE setStarPowerEnabled NOTIFY starPowerEnabledChanged)
    Q_PROPERTY(bool starPowerPhraseEnabled READ getStarPowerPhraseEnabled WRITE setStarPowerPhraseEnabled NOTIFY starPowerPhraseEnabledChanged)
    Q_PROPERTY(int openColor READ getOpenColor WRITE setOpenColor NOTIFY openColorChanged)
    Q_PROPERTY(int starPowerColor READ getStarPowerColor WRITE setStarPowerColor NOTIFY starPowerColorChanged)
    Q_PROPERTY(int starPowerPhraseColor READ getStarPowerPhraseColor WRITE setStarPowerPhraseColor NOTIFY starPowerPhraseColorChanged)
    Q_PROPERTY(QStringList supportedVersions MEMBER m_supportedVersions NOTIFY supportedVersionsChanged)
   public:
    explicit LEDHandler(QGuiApplication *application, PortScanner *scanner, QObject *parent = nullptr);
    QTimer *timer;
    QString getGameFolder() {
        return m_gameFolder;
    }
    bool getStarPowerEnabled() {
        return m_starPowerEnabled;
    }
    bool getStarPowerPhraseEnabled() {
        return m_starPowerPhraseEnabled;
    }
    bool getOpenEnabled() {
        return m_openEnabled;
    }
    int getOpenColor() {
        return m_open;
    }
    int getStarPowerColor() {
        return m_star_power;
    }
    int getStarPowerPhraseColor() {
        return m_star_power_phrase;
    }
    QString getVersion() {
        return m_version;
    }
   signals:
    void gameFolderChanged();
    void versionChanged();
    void readyChanged();
    void colorChanged();
    void starPowerEnabledChanged();
    void starPowerPhraseEnabledChanged();

    void openEnabledChanged();
    void starPowerColorChanged();
    void starPowerPhraseColorChanged();
    void openColorChanged();
    void hitColorChanged();
    void supportedVersionsChanged();
   public slots:
    int gammaCorrect(int color);
    void startGame();
    void setColors(int color, QStringList buttons);
    void setColor(int rgb, QString button);
    void setColors(QMap<QString, uint32_t> colours);
   private slots:
    void tick();
    void setGameFolder(QString string);
    void setOpenEnabled(bool open);
    void setStarPowerEnabled(bool star);
    void setStarPowerPhraseEnabled(bool star);
    void setStarPowerPhraseColor(int color);
    void setStarPowerColor(int color);
    void setOpenColor(int color);
    void setVersion(QString version);

   private:
    QSettings settings;
    QNetworkAccessManager m_WebCtrl;
    QString binary;
    Q_PID pid;
    QString m_version;
    QList<CloneHeroBundle> bundles;
    QProcess process;
    PortScanner *scanner;
    int lastScore = 0;
    int shownNote = 0;
    int countdown = 0;
    bool m_starPowerEnabled;
    bool m_starPowerPhraseEnabled;
    bool m_openEnabled;
    bool m_ready;
    uint32_t m_star_power;
    uint32_t m_star_power_phrase;
    uint32_t m_open;
    QString m_gameFolder;
    QMap<QString, uint32_t> lastData;
    qint64 base = 0;
    qint64 readFromProc(quint64, qint64 addr, qint64 *buf);
    qint64 readData(qint64 base, QList<qint64> &path, qint64 pathCount, qint64 *buf);
    void updateVersion();
    QList<qint64> pointerPathBasePlayer;
    QList<qint64> pointerPathCurrentNote;
    QStringList m_supportedVersions;

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
    QFile *inputFile;
#endif
#if defined Q_OS_MAC
    mach_port_t task;
#endif
};

#endif  // CLONEHEROINFO_H
