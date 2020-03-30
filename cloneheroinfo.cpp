#include "cloneheroinfo.h"
#include <QFile>
#include "qprocessinfo.h"
#include <proc/readproc.h>
#include <proc/version.h>

CloneHeroInfo::CloneHeroInfo(QObject *parent) : QObject(parent)
{
    QString platform;
#ifdef Q_OS_WIN64
    platform = "win64";
    binary = "Clone Hero.exe";
#elif defined Q_OS_WIN32
    platform = "win32";
    binary = "Clone Hero.exe";
#elif defined Q_OS_MACOS
    platform = "mac";
    binary = "Clone Hero";
#elif defined Q_OS_LINUX
    platform = "linux";
    binary = "clonehero";
#endif
    QString url = "https://dltest.b-cdn.net/"+platform+"-index.json";
    connect(&m_WebCtrl, &QNetworkAccessManager::finished, this, &CloneHeroInfo::fileDownloaded);
    QNetworkRequest r(url);
    m_WebCtrl.get(r);
    url = "https://dlpublic.b-cdn.net/"+platform+"-index.json";
    QNetworkRequest r2(url);
    m_WebCtrl.get(r2);
    // create a timer
    timer = new QTimer(this);

    // setup signal and slot
    connect(timer, &QTimer::timeout,  this, &CloneHeroInfo::tick);

    // msec
    timer->start(1);
}

void CloneHeroInfo::fileDownloaded(QNetworkReply* pReply) {
 QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
 QJsonArray arr = doc.array();
 for (auto v: arr) {
     auto o = v.toObject();
     hashes[o["hash"].toArray()[0].toArray()[1].toString()] = o["version"].toString();
 }
}
QFile* inputFile;
QString mapbuf;
qint64 base;
QString libLinux = "UnityPlayer.so";
QString libWindows = "UnityPlayer.dll";
#define SIZE_ARR(arr) sizeof(arr)/sizeof(arr[0])
#define SIZE 0xFFF
qint64 readFromProc(qint64 size, qint64 addr, qint64 *buf)
{
    inputFile->seek((long)addr);
    return inputFile->read((char*)buf, size);
}
qint64 readData(qint64 base, qint64 *path, qint64 pathCount, qint64 *buf) {
    qint64 addr;
    if (readFromProc(sizeof(qint64), base + path[0], buf) == -1)
    {
        return -1;
    }
    for (int i = 1; i < pathCount; i++)
    {
        addr = buf[0] + path[i];
        if (readFromProc(sizeof(qint64), addr, buf) == -1)
        {
            return -1;
        }
    }
    readFromProc(SIZE, addr, buf);
    return (qint64)addr;
}
qint64 pointerPathBasePlayerWindows[] = {0x01792990, 0x150, 0x1E0, 0x78, 0x48, 0x20, 0x00};
qint64 pointerPathBasePlayerLinux[] = {0x020C7530, 0xF30, 0x170, 0x28,0x178, 0x00};
qint64 pointerPathCurrentNote[] = {0xB8,0x20,0x00};

// int32_t
int offsetButtonsPressed = 0xE0;
// bool
int offsetStarPowerActivated = 0xCE;
// bool
int offsetIsStarPower = 0xF0;

int offsetScore = 0x120;
// int32_t
int offsetCurrentNote = 0x28;
int lastScore = 0;
void CloneHeroInfo::tick() {
    if (!inputFile || !inputFile->isReadable()) {
        QProcessInfo q;
        for (auto p : QProcessInfo::enumerate(false)) {
            if (p.name() == binary) {
                QFile f(p.command());
                f.open(QFile::ReadOnly);
                pid = p.pid();
                inputFile = new QFile(QStringLiteral("/proc/%1/maps").arg(pid));
                inputFile->open(QIODevice::ReadOnly);
                if (!inputFile->isOpen()) {
                    return;
                }

                QTextStream stream(inputFile);
                QString line;
                while (stream.readLineInto(&line)) {
                    if (line.contains(libLinux)) {
                        bool okay = true;
                        base = line.split("-")[0].toLong(&okay, 16);
                        break;
                    }
                };
                inputFile->close();
                inputFile = new QFile(QStringLiteral("/proc/%1/mem").arg(pid));
                inputFile->open(QIODevice::ReadOnly);
                if (!inputFile->isOpen())
                    return;
                version = QString(QCryptographicHash::hash(f.readAll(),QCryptographicHash::Md5).toHex());
            }
        }
        return;
    }
    qint64 buf[SIZE];
    char *cbuf = (char *)buf;
    qint64 addr = readData(base, pointerPathBasePlayerLinux, SIZE_ARR(pointerPathBasePlayerLinux), buf);
    int score = *(size_t*)(cbuf+offsetScore);
    if (score > lastScore) {
        qDebug() << "StarPower: "<< *(cbuf+offsetIsStarPower);
        addr = readData(addr, pointerPathCurrentNote, SIZE_ARR(pointerPathCurrentNote), buf);
        qDebug() << "Notes: " << QString::number(*(cbuf+offsetCurrentNote),2).rightJustified(8, '0');;
    }
    lastScore = score;

}



