#include "ledhandler.h"

#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QProcess>
#include <QThread>
#ifdef Q_OS_LINUX
#include <proc/readproc.h>
#include <proc/version.h>
#elif defined(Q_OS_WIN)
#include <Psapi.h>
#include <Windows.h>
#endif
LEDHandler::LEDHandler(QGuiApplication *application, PortScanner *scanner, QObject *parent) : QObject(parent), scanner(scanner), m_gameFolder(""), m_version("") {
#ifdef Q_OS_WIN64
    platform = "win64";
    binary = "Clone Hero.exe";
#elif defined Q_OS_WIN32
    platform = "win32";
    binary = "Clone Hero.exe";
#elif defined Q_OS_MACOS
    platform = "mac";
    binary = "Clone Hero.app/Contents/MacOS/Clone Hero";
#elif defined Q_OS_LINUX
    platform = "linux";
    binary = "clonehero";
#endif
    connect(application, &QGuiApplication::aboutToQuit, &process, &QProcess::terminate);
    if (settings.contains("cloneHeroDir")) {
        m_gameFolder = settings.value("cloneHeroDir").toString();
    }
    if (settings.contains("version")) {
        m_version = settings.value("version").toString();
    }
    auto dir = QDir(QCoreApplication::applicationDirPath());
    QFile memLoc(dir.filePath("memory-locations.json"));
    memLoc.open(QFile::ReadOnly | QIODevice::Text);
    QJsonDocument doc = QJsonDocument::fromJson(memLoc.readAll());
    QJsonObject obj = doc.object();
    m_supportedVersions = obj.keys();
    memLoc.close();
    updateVersion();
    m_star_power = settings.value("led_star_power_color", QVariant(0x00BFFF)).toUInt();
    m_open = settings.value("led_open_color", QVariant(0xFF00FF)).toUInt();
    m_openEnabled = settings.contains("led_open") && settings.value("led_open").toBool();
    m_starPowerEnabled = settings.contains("led_star_power") && settings.value("led_star_power").toBool();
}
void readList(QJsonArray arr, QList<qint64> *list) {
    for (auto a : arr) {
        list->push_back(a.toVariant().toLongLong());
    }
}
void LEDHandler::updateVersion() {
    if (!m_gameFolder.isEmpty() && !m_version.isEmpty()) {
        m_ready = true;
        auto dir = QDir(QCoreApplication::applicationDirPath());
        QFile memLoc(dir.filePath("memory-locations.json"));
        memLoc.open(QFile::ReadOnly | QIODevice::Text);
        QJsonDocument doc = QJsonDocument::fromJson(memLoc.readAll());
        QJsonObject obj = doc.object();
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
        memLoc.close();
    }
}
void LEDHandler::setOpenColor(int color) {
    m_open = color;
    settings.setValue("led_open_color", color);
    openColorChanged();
}
void LEDHandler::setStarPowerColor(int color) {
    m_star_power = color;
    settings.setValue("led_star_power_color", color);
    starPowerColorChanged();
}
void LEDHandler::setOpenEnabled(bool open) {
    m_openEnabled = open;
    settings.setValue("led_open", open);
    openEnabledChanged();
}
void LEDHandler::setStarPowerEnabled(bool hit) {
    m_starPowerEnabled = hit;
    settings.setValue("led_star_power", hit);
    starPowerEnabledChanged();
}
void LEDHandler::setGameFolder(QString gameFolder) {
    m_gameFolder = QDir::toNativeSeparators(QUrl(gameFolder).toLocalFile());
    settings.setValue("cloneHeroDir", m_gameFolder);
    gameFolderChanged();
    if (!m_gameFolder.isEmpty() && !m_version.isEmpty()) {
        m_ready = true;
    }
    readyChanged();
}
void LEDHandler::setVersion(QString version) {
    m_version = version;
    settings.setValue("version", version);
    if (!m_gameFolder.isEmpty() && !m_version.isEmpty()) {
        m_ready = true;
    }
    readyChanged();
    versionChanged();
}
int LEDHandler::gammaCorrect(int color) {
    uint32_t ucolor = color;
    //Gamma-correct values
    uint8_t r = ucolor >> 16 & 0xff;
    uint8_t g = ucolor >> 8 & 0xff;
    uint8_t b = ucolor & 0xff;
    r = (pow(r / 255.0, 2.8) * 255 + 0.5);
    g = (pow(g / 255.0, 2.8) * 255 + 0.5);
    b = (pow(b / 255.0, 2.8) * 255 + 0.5);
    return r << 16 | g << 8 | b;
}
void LEDHandler::setColor(int color, QString button) {
    auto m = QMap<QString, uint32_t>();
    m[button] = color;
    setColors(m);
}
void LEDHandler::setColors(int color, QStringList buttons) {
    auto m = QMap<QString, uint32_t>();
    for (auto button : buttons) {
        m[button] = color;
    }
    setColors(m);
}
void LEDHandler::setColors(QMap<QString, uint32_t> buttons) {
    QByteArray data;
    Ardwiino *dev = static_cast<Ardwiino *>(scanner->getSelected());
    auto mappings = dev->getConfig()->getMappings();
    auto leds = dev->getConfig()->getLEDs();
    for (auto led : leds) {
        if (buttons.contains(led.toString())) {
            auto color = buttons[led.toString()];
            uint8_t r = color >> 16 & 0xff;
            uint8_t g = color >> 8 & 0xff;
            uint8_t b = color & 0xff;
            data.push_back(mappings[led.toString()]);
            data.push_back(r);
            data.push_back(g);
            data.push_back(b);
        } else {
            data.push_back(mappings[led.toString()]);
            data.push_back('\0');
            data.push_back('\0');
            data.push_back('\0');
        }
    }
    data.push_back('\0');
    dev->writeChunked(COMMAND_SET_LEDS, data);
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
    EnumProcessModulesEx(pid->hProcess, handles, sizeof(handles), &needed, LIST_MODULES_ALL);
    for (uint i = 0; i < needed / sizeof(handles[0]); i++) {
        char name[1024];
        GetModuleBaseNameA(pid->hProcess, handles[i], name, sizeof(name));
        if (QString(name).endsWith(lib)) {
            base = (qint64) handles[i];
            break;
        }
    }
#endif
#if defined Q_OS_MAC
    QProcess vmem;
    vmem.start("vmmap", {QString::number(pid)});
    vmem.waitForFinished();
    for (QString line : vmem.readAllStandardOutput().split('\n')) {
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
    connect(timer, &QTimer::timeout, this, &LEDHandler::tick);

    // msec
    timer->start(1);
}
size_t _word_align(size_t size) {
    size_t rsize = 0;

    rsize = ((size % sizeof(long)) > 0) ? (sizeof(long) - (size % sizeof(long))) : 0;
    rsize += size;

    return rsize;
}
qint64 LEDHandler::readFromProc(quint64 size, qint64 addr, qint64 *buf) {
#if defined Q_OS_LINUX
    inputFile->seek((long)addr);
    return inputFile->read((char *)buf, size);
#endif

#if defined Q_OS_WIN
    SIZE_T read;
    if (!ReadProcessMemory(pid->hProcess, (void*)addr, buf, size, &read)) {
        auto err = GetLastError();
        if (err == ERROR_PARTIAL_COPY) {
            return qint64(read);
        }
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
    if ((ret = readFromProc(sizeof(qint64), base + path[0], buf)) < 0) {
        return ret;
    }
    for (int i = 1; i < pathCount; i++) {
        addr = buf[0] + path[i];
        if ((ret = readFromProc(sizeof(qint64), addr, buf)) < 0) {
            return ret;
        }
    }
    readFromProc(512, addr, buf);
    return (qint64)addr;
}
bool lastSP = false;
void LEDHandler::tick() {
    qint64 buf[512];
    char *cbuf = (char *)buf;
    qint64 addr = readData(base, pointerPathBasePlayer, pointerPathBasePlayer.length(), buf);
    int score = *(size_t *)(cbuf + offsetScore);
    bool noteIsStarPower = *(cbuf + offsetIsStarPower);
    bool starPowerActivated = *(cbuf + offsetStarPowerActivated);
    uint8_t buttons = *(uint8_t *)(cbuf + offsetButtonsPressed);
    if (addr > 0) {
        addr = readData(addr, pointerPathCurrentNote, pointerPathCurrentNote.length(), buf);
    }
    uint8_t lastNote = *(uint8_t *)(cbuf + offsetCurrentNote);
    if (score > lastScore && lastNote & 1 << 6) {
        shownNote = lastNote;
        countdown = 2;
    }
    if (lastSP != (noteIsStarPower || starPowerActivated)) {
        lastSP = noteIsStarPower || starPowerActivated;
        Ardwiino *dev = static_cast<Ardwiino *>(scanner->getSelected());
        dev->writeChunked(COMMAND_SET_SP, QByteArray(1, lastSP));
    }
    QMap<QString, uint32_t> data;
    QStringList names = {"A", "B", "Y", "X", "LB"};
    Ardwiino *dev = static_cast<Ardwiino *>(scanner->getSelected());
    for (int i = 0; i < 5; i++) {
        if (addr > 0) {
            if (countdown > 0 && shownNote & 1 << 6) {
                data[names[i]] = (noteIsStarPower || starPowerActivated) ? m_star_power : m_open;
            } else if (buttons & 1 << i) {
                if (score > lastScore && lastNote & 1 << i) {
                    shownNote = lastNote;
                    countdown = 0;
                }
                if (shownNote & 1 << i) {
                    data[names[i]] = (noteIsStarPower && !starPowerActivated) ? m_star_power : dev->getConfig()->getLEDColours()[names[i]].toUInt();
                }
            } else if (starPowerActivated) {
                data[names[i]] = m_star_power;
            } else {
                data[names[i]] = 0;
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
        for (int i = 0; i < 5; i++) {
            data[names[i]] = 0;
            dev->writeChunked(COMMAND_SET_SP, QByteArray(1, 0));
        }
        disconnect(timer, &QTimer::timeout, this, &LEDHandler::tick);
    }
    if (lastData.values() != data.values()) {
        setColors(data);
    }
    lastData = data;
}
