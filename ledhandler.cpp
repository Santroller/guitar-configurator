#include "ledhandler.h"
#include <QFile>
#include <QProcess>
#include <QGuiApplication>
#include <QDir>
#include <QThread>
#if defined Q_OS_LINUX
    #include <proc/readproc.h>
    #include <proc/version.h>
#endif
#if defined Q_OS_WIN
    #include <Windows.h>
    #include <Psapi.h>
#endif
LEDHandler::LEDHandler(QGuiApplication* application, PortScanner* scanner, QObject *parent) : QObject(parent), scanner(scanner)
{
#ifdef Q_OS_WIN64
    platform = "win64";
#elif defined Q_OS_WIN32
    platform = "win32";
#elif defined Q_OS_MACOS
    platform = "mac";
#elif defined Q_OS_LINUX
    platform = "linux";
#endif
    auto dir = QDir(QCoreApplication::applicationDirPath());
    QFile prod(dir.filePath("ch-index.json"));
    QFile test(dir.filePath("ch-index-test.json"));
    QFile* indices[] = {&prod,&test};
    for (auto &a : indices) {
        a->open(QFile::ReadOnly| QIODevice::Text);
        QJsonDocument doc = QJsonDocument::fromJson(a->readAll());
        QJsonArray arr = doc.array();
        if (hashedFiles.empty()) {
            for (auto hashObj: arr[0].toObject()["hash"].toArray()) {
                hashedFiles.push_back(hashObj.toArray()[0].toString());
            }
            binary = arr[0].toObject()["hash"].toArray()[0].toArray()[0].toString().remove(0,1);
        }
        for (auto v: arr) {
            auto o = v.toObject();
            QString hash;
            for (auto hashObj: o["hash"].toArray()) {
                hash += hashObj.toArray()[1].toString();
            }
            hashes[hash] = o["version"].toString();
        }
        a->close();
    }
    connect(application, &QGuiApplication::aboutToQuit, &process, &QProcess::terminate);
    if (settings.contains("cloneHeroDir")) {
        m_gameFolder = settings.value("cloneHeroDir").toString();
    }
    m_hitEnabled = settings.contains("led_hit") && settings.value("led_hit").toBool();
    m_openEnabled = settings.contains("led_open") && settings.value("led_open").toBool();
    m_starPowerEnabled = settings.contains("led_star_power") && settings.value("led_star_power").toBool();
    findVersion();
}
void LEDHandler::setHitEnabled(bool hit) {
    m_hitEnabled = hit;
    settings.setValue("led_hit",hit);
    hitEnabledChanged();
}
void LEDHandler::setOpenEnabled(bool open) {
    m_openEnabled = open;
    settings.setValue("led_open",open);
    openEnabledChanged();
}
void LEDHandler::setStarPowerEnabled(bool hit) {
    m_starPowerEnabled = hit;
    settings.setValue("led_star_power",hit);
    starPowerEnabledChanged();
}
void LEDHandler::setGameFolder(QString gameFolder) {
    gameFolder = gameFolder.replace("file://","");
    m_gameFolder = gameFolder;
    settings.setValue("cloneHeroDir",gameFolder);
    findVersion();
    gameFolderChanged();
}
void readList(QJsonArray arr, QList<qint64>* list) {
    for (auto a: arr) {
        list->push_back(a.toVariant().toLongLong());
    }
}
void LEDHandler::setColor(int color) {
    uint32_t ucolor = color;
    //Gamma-correct values
    uint8_t r = ucolor >> 16 & 0xff;
    uint8_t g = ucolor >> 8 & 0xff;
    uint8_t b = ucolor & 0xff;
    r = (pow(r / 255.0, 2.8) * 255 + 0.5);
    g = (pow(g / 255.0, 2.8) * 255 + 0.5);
    b = (pow(b / 255.0, 2.8) * 255 + 0.5);
    if (scanner->selectedPort() && scanner->selectedPort()->isReady()) {
        QByteArray a = QByteArray(1,COMMAND_SET_LED_COLOUR);
        a.append((char)b).append((char)g).append((char)r);
        scanner->selectedPort()->write(a);
    }
    m_color = color;
}
void LEDHandler::findVersion() {
    QString hash;
    for (QString file: hashedFiles) {
        QString filePath = m_gameFolder + file;
        QFile f(filePath);
        if (f.exists()) {
            f.open(QFile::ReadOnly);
            hash += QString(QCryptographicHash::hash(f.readAll(),QCryptographicHash::Md5).toHex());
            f.close();
        } else {
            m_version = "Unable to locate game executable";
        }
        f.close();
    }
    if (hashes.contains(hash)) {
        m_version = hashes[hash];
    } else {
        m_version = "Unknown version!";
    }
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
        m_ready = true;
    } else {
        m_version = "Unsupported Version: "+m_version;
        m_ready = false;
    }
    memLoc.close();
    readyChanged();
    versionChanged();
}

void LEDHandler::startGame() {
    if (process.pid() != 0) {
        return;
    }
    QString binaryPath = QDir(m_gameFolder).filePath(binary);
    process.start(binaryPath, {"--launcher-build"});
    process.waitForStarted();
    QThread::msleep(500);
    pid = process.pid();
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
#if defined Q_OS_WIN
    HMODULE handles[2048];
    DWORD needed;
    EnumProcessModules(pid->hProcess, handles, sizeof(handles), &needed);
    for (int i = 0; i < needed / sizeof(handles[0]); i++) {
        MODULEINFO info;
        char name[1024];
        GetModuleBaseNameA(pid->hProcess, handles[i], name, sizeof(name));
        if (QString(name).endsWith(lib)) {
            GetModuleInformation(pid->hProcess, handles[i], &info, sizeof(info));
            base = (qint64)info.lpBaseOfDll;
            break;
        }
    }
#endif
#if defined Q_OS_MAC
    QProcess vmem;
    vmem.start("vmmap", {QString::number(pid)});
    vmem.waitForFinished();
    for (QString line :vmem.readAllStandardOutput().split('\n')) {
        if (line.indexOf(lib) != -1) {
            bool okay = true;
            base = line.split(QRegExp("\\s+"))[1].split('-')[0].toLong(&okay, 16);
            break;
        }
    }

    kern_return_t kernret = task_for_pid(mach_task_self(), pid, &task);
    if (kernret != KERN_SUCCESS) {
        // TODO: give the user a warning here!
        qDebug() << "Unable to get task from pid!";
        return;
    }
#endif

    // create a timer
    timer = new QTimer(this);

    // setup signal and slot
    connect(timer, &QTimer::timeout,  this, &LEDHandler::tick);

    // msec
    timer->start(1);
}
size_t _word_align(size_t size)
{
    size_t rsize = 0;

    rsize = ((size % sizeof(long)) > 0) ? (sizeof(long) - (size % sizeof(long))) : 0;
    rsize += size;

    return rsize;
}
qint64 LEDHandler::readFromProc(quint64 size, qint64 addr, qint64 *buf)
{

#if defined Q_OS_LINUX
    inputFile->seek((long)addr);
    return inputFile->read((char*)buf, size);
#endif

#if defined Q_OS_WIN
    quint64 read;
    if (!ReadProcessMemory(pid->hProcess, reinterpret_cast<const char *>(addr), buf, size, &read)) {
        return -1;
    }
    return qint64(read);
#endif
#if defined Q_OS_MAC
        size = _word_align(size);
        vm_size_t data_cnt;
        kern_return_t kernret = vm_read_overwrite(task, (vm_address_t)addr, size, (vm_address_t)buf, &data_cnt);
        if (kernret == KERN_SUCCESS) return kernret;
        return -1;

#endif
}
qint64 LEDHandler::readData(qint64 base, QList<qint64> &path, qint64 pathCount, qint64 *buf) {
    qint64 addr;
    qint64 ret;
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
    readFromProc(512, addr, buf);
    return (qint64)addr;
}

void LEDHandler::tick() {
    qint64 buf[512];
    char *cbuf = (char *)buf;
    qint64 addr = readData(base, pointerPathBasePlayer, pointerPathBasePlayer.length(), buf);
    QByteArray data(1,COMMAND_SET_GH_LEDS);
    int score = *(size_t*)(cbuf+offsetScore);
    bool noteIsStarPower = *(cbuf+offsetIsStarPower);
    bool starPowerActivated = *(cbuf+offsetStarPowerActivated);
    uint8_t buttons = *(uint8_t*)(cbuf+offsetButtonsPressed);
    if (addr > 0) {
        addr = readData(addr, pointerPathCurrentNote, pointerPathCurrentNote.length(), buf);
    }
    uint8_t lastNote = *(uint8_t*)(cbuf+offsetCurrentNote);
    if (score > lastScore && lastNote & 1<<6) {
        shownNote = lastNote;
        countdown = 2;
    }
    for (int i =0; i < 5; i++) {
        if (addr < 0) {
            data.push_back((char)0);
        } else {
            if (countdown > 0 && shownNote & 1<<6) {
                data.push_back((noteIsStarPower || starPowerActivated)?3:4);
            } else if (buttons & 1<<i) {
                if (score > lastScore && lastNote & 1<<i) {
                    shownNote = lastNote;
                    countdown = 1;
                }
                if (countdown > 0 && shownNote & 1<<i) {
                    data.push_back((noteIsStarPower && !starPowerActivated)?3:2);
                } else {
                    data.push_back(starPowerActivated?2:1);
                }
            } else {
                data.push_back(starPowerActivated?3:0);
            }
        }
    }
    lastScore = score;
    countdown--;
    if (!process.pid()) {
#if defined Q_OS_LINUX
        inputFile->close();
        inputFile = nullptr;
#endif
        disconnect(timer, &QTimer::timeout,  this, &LEDHandler::tick);
    }
    if (scanner->selectedPort() && scanner->selectedPort()->isReady() && data != lastData) {
        scanner->selectedPort()->write(data);
        lastData = data;
    }

}



