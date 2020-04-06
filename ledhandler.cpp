#include "ledhandler.h"
#include <QFile>
#include <proc/readproc.h>
#include <proc/version.h>
#include <QProcess>
#include <QGuiApplication>
#include <QDir>

LEDHandler::LEDHandler(QGuiApplication* application, PortScanner* scanner, QObject *parent) : QObject(parent), scanner(scanner)
{
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
    auto dir = QDir(QCoreApplication::applicationDirPath());
    QFile prod(dir.filePath("ch-index.json"));
    QFile test(dir.filePath("ch-index-test.json"));
    QFile* indices[] = {&prod,&test};
    for (auto &a : indices) {
        a->open(QFile::ReadOnly| QIODevice::Text);
        QJsonDocument doc = QJsonDocument::fromJson(a->readAll());
        QJsonArray arr = doc.array();
        for (auto v: arr) {
            auto o = v.toObject();
            hashes[o["hash"].toArray()[0].toArray()[1].toString()] = o["version"].toString();
        }
        a->close();
    }
    connect(application, &QGuiApplication::aboutToQuit, &process, &QProcess::terminate);
    if (settings.contains("cloneHeroDir")) {
        m_gameFolder = settings.value("cloneHeroDir").toString();
    }
    findVersion();
}
void LEDHandler::setGameFolder(QString gameFolder) {
    m_gameFolder = gameFolder;
    settings.setValue("cloneHeroDir",gameFolder);
    gameFolderChanged();
}
void readList(QJsonArray arr, QList<qint64>* list) {
    for (auto a: arr) {
        list->push_back(a.toVariant().toLongLong());
    }
}
void LEDHandler::findVersion() {
    QString binaryPath = QDir(m_gameFolder).filePath(binary);
    QFile f(binaryPath);
    if (f.exists()) {
        f.open(QFile::ReadOnly);
        m_version = QString(QCryptographicHash::hash(f.readAll(),QCryptographicHash::Md5).toHex());
        if (hashes.contains(m_version)) {
            m_version = hashes[m_version];
        } else {
            m_version = "Unknown version!";
        }
        f.close();
    } else {
        m_version = "Unable to locate game executable";
    }
    f.close();
    auto dir = QDir(QCoreApplication::applicationDirPath());
    QFile memLoc(dir.filePath("memory-locations.json"));
    memLoc.open(QFile::ReadOnly| QIODevice::Text);
    QJsonDocument doc = QJsonDocument::fromJson(memLoc.readAll());
    QJsonObject obj = doc.object();
    if (obj.contains(m_version)) {
        obj = obj[m_version].toObject();
        QJsonObject osObj = obj[platform].toObject();
        lib = osObj["lib"].toString();
        readList(osObj["pointerPathBasePlayer"].toArray(), &pointerPathBasePlayer);
        readList(obj["pointerPathCurrentNote"].toArray(), &pointerPathCurrentNote);
        offsetButtonsPressed = obj["offsetButtonsPressed"].toInt();
        offsetStarPowerActivated = obj["offsetStarPowerActivated"].toInt();
        offsetIsStarPower = obj["offsetIsStarPower"].toInt();
        offsetScore = obj["offsetScore"].toInt();
        offsetCurrentNote = obj["offsetCurrentNote"].toInt();
        maxOffset = std::max({offsetScore, offsetCurrentNote, offsetIsStarPower, offsetButtonsPressed, offsetStarPowerActivated});
    } else {
        m_version = "Unsupported Version: "+m_version;
    }
    memLoc.close();
    versionChanged();
}

void LEDHandler::startGame() {
    if (process.pid() != 0) {
        return;
    }
    QString binaryPath = QDir(m_gameFolder).filePath(binary);
    process.start(binaryPath, {"--launcher-build"});
    process.waitForStarted();
    process.waitForReadyRead();
    pid = process.pid();
    //TODO: this, and readFromProc need to be ported to linux + windows.
#if defined Q_OS_LINUX
    inputFile = new QFile(QStringLiteral("/proc/%1/maps").arg(pid));
    inputFile->open(QIODevice::ReadOnly);
    if (!inputFile->isOpen()) {
        return;
    }

    QTextStream stream(inputFile);
    QString line;
    while (stream.readLineInto(&line)) {
        if (line.contains(lib)) {
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
#endif
    // create a timer
    timer = new QTimer(this);

    // setup signal and slot
    connect(timer, &QTimer::timeout,  this, &LEDHandler::tick);

    // msec
    timer->start(1);
}

qint64 LEDHandler::readFromProc(qint64 size, qint64 addr, qint64 *buf)
{

#if defined Q_OS_LINUX
    inputFile->seek((long)addr);
    return inputFile->read((char*)buf, size);
#endif
}
qint64 LEDHandler::readData(qint64 base, QList<qint64> &path, qint64 pathCount, qint64 *buf) {
    qint64 addr;
    int ret;
    if ((ret = readFromProc(sizeof(qint64), base + path[0], buf)) < 0)
    {
        return ret;
    }
    for (int i = 1; i < pathCount; i++)
    {
        addr = buf[0] + path[i];
        if ((ret = readFromProc(sizeof(qint64), addr, buf)) < 0)
        {
            return ret;
        }
    }
    readFromProc(maxOffset, addr, buf);
    return (qint64)addr;
}

void LEDHandler::tick() {
    qint64 buf[maxOffset];
    char *cbuf = (char *)buf;
    qint64 addr = readData(base, pointerPathBasePlayer, pointerPathBasePlayer.length(), buf);
    int score = *(size_t*)(cbuf+offsetScore);
    bool noteIsStarPower = *(cbuf+offsetIsStarPower);
    bool starPowerActivated = *(cbuf+offsetStarPowerActivated);
    uint8_t buttons = *(uint8_t*)(cbuf+offsetButtonsPressed);
    addr = readData(addr, pointerPathCurrentNote, pointerPathCurrentNote.length(), buf);
    uint8_t lastNote = *(uint8_t*)(cbuf+offsetCurrentNote);
    QByteArray data("l");
    if (score > lastScore && lastNote & 1<<6) {
        shownNote = lastNote;
        countdown = 1;
    }
    for (int i =0; i < 5; i++) {
        if (countdown > 0 && shownNote & 1<<6) {
            data.push_back((noteIsStarPower || starPowerActivated)?"3":"4");
        } else if (buttons & 1<<i) {
            if (score > lastScore && lastNote & 1<<i) {
                shownNote = lastNote;
                countdown = 1;
            }
            if (countdown > 0 && shownNote & 1<<i) {
                data.push_back((noteIsStarPower && !starPowerActivated)?"3":"2");
            } else {
                data.push_back(starPowerActivated?"2":"1");
            }
        } else {
            data.push_back(starPowerActivated?"3":"0");
        }
    }
    //Show nothing if in menu.
    if (process.pid() == 0) {
        lastData = "";
        data = "l00000";
        inputFile->close();
        inputFile = nullptr;
        disconnect(timer, &QTimer::timeout,  this, &LEDHandler::tick);
    }
    if (scanner->selectedPort() && scanner->selectedPort()->isReady() && data != lastData) {
        scanner->selectedPort()->write(data);
        lastData = data;
    }
    lastScore = score;
    countdown--;

}



