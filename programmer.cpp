#include "programmer.h"

#include <ardwiinolookup.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QRegularExpression>
#include <QStringList>
#include <QtEndian>
#include <algorithm>
#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <vector>

Programmer::Programmer(QObject *parent) : QObject(parent), m_status(Status::NOT_PROGRAMMING), m_device(nullptr), m_restore(false), m_rf(false), m_isGeneric(false), m_count(0), m_step(0), m_steps(0) {
}
void Programmer::prepareRF(Ardwiino *device) {
    m_rf = true;
    m_parent_device = device;
}
void Programmer::deviceAdded(DfuArduino *device) {
    if (m_status != Status::NOT_PROGRAMMING) {
        QString board = m_device->getBoard().shortName;
        if (!board.contains("-")) {
            board += "-" + device->getProcessor();
        }
        device->setBoardType(board);
    }
    m_device = device;
    switch (m_status) {
        case Status::DFU_CONNECT_AVRDUDE:
            m_status = Status::DFU_ERASE_AVRDUDE;
            programDFU();
            break;
        case Status::DFU_CONNECT_MAIN:
            m_status = Status::DFU_ERASE_MAIN;
            programDFU();
            break;
    }
    emit statusChanged(m_status);
    emit statusVChanged(getStatusDescription());
}
void Programmer::deviceAdded(Ardwiino *device) {
    m_device = device;
    if (m_status == Status::DFU_DISCONNECT_MAIN || m_status == Status::DISCONNECT_AVRDUDE || m_status == Status::DISCONNECT_PICOBOOT) {
        m_status = Status::COMPLETE;
        if (m_rf) {
            m_rf = false;
            m_device = m_parent_device;
            m_parent_device->writeConfig();
            m_parent_device = NULL;
        }
        emit statusChanged(m_status);
        emit statusVChanged(getStatusDescription());
    }
}

void Programmer::programPico() {
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("firmware");
    QString uf2File = "ardwiino-pico-rp2040";
    QFile file(dir.filePath(uf2File + (m_rf ? "-rf" : "") + ".uf2"));
    auto f = [=](long a, long b, int step, int stepCount) {
        return this->setPercentage(a, b, step, stepCount);
    };
    ((PicobootDevice *)m_device)->program(&file, m_parent_device, f);
}
void Programmer::deviceAdded(PicobootDevice *device) {
    m_device = device;
    if (m_status == Status::WAIT_PICOBOOT) {
        m_status = Status::PICOBOOT;
        programPico();
    }
}
void Programmer::deviceAdded(Arduino *device) {
    if (m_status != Status::NOT_PROGRAMMING && m_device) {
        if (!m_device->getBoard().hasDFU) {
            device->setBoardType(m_device->getBoard().shortName, m_device->getBoard().cpuFrequency);
        } else {
            device->setBoardType(m_device->getBoard().shortName.split("-")[0], m_device->getBoard().cpuFrequency);
        }
    }
    m_device = device;
    if (m_status == Status::DFU_DISCONNECT_AVRDUDE || m_status == Status::WAIT_AVRDUDE) {
        if (m_restore && m_device->getBoard().hasDFU) {
            m_status = Status::COMPLETE;
        } else {
            m_status = Status::AVRDUDE;
            programAvrDude();
        }
    }
    if (m_restore && m_status == Status::DISCONNECT_AVRDUDE && !m_device->getBoard().hasDFU) {
        m_status = Status::COMPLETE;
    }
    emit statusChanged(m_status);
    emit statusVChanged(getStatusDescription());
}
void Programmer::programDFU() {
    board_t board = m_device->getBoard();
    QString hexFile = "ardwiino-" + board.hexFile + "-" + board.processor + "-" + QString::number(board.cpuFrequency);
    if (m_restore) {
        hexFile += "-restore";
    } else if (m_status == Status::DFU_FLASH_AVRDUDE) {
        hexFile += "-usbserial";
    }
    hexFile += ".hex";
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("firmware");
    m_process_out.clear();
    QString file = dir.filePath(hexFile);
    dir.cdUp();
    dir.cd("binaries");
    m_process = new QProcess();
    m_process->setWorkingDirectory(dir.path());
    UsbDevice_t usb = m_device->getUSBDevice();
    // Atmel devices use libusb-0.1 drivers, and these do not appear to expose a port number on windows.
#ifndef Q_OS_WIN
    QStringList l = {
        board.processor + ":" + QVariant(usb.bus).toString() + "," + QVariant(usb.port).toString()};
#else
    QStringList l = {board.processor};
#endif
    switch (m_status) {
        case Status::DFU_ERASE_MAIN:
        case Status::DFU_ERASE_AVRDUDE:
            l.push_back("erase");
            break;
        case Status::DFU_FLASH_AVRDUDE:
        case Status::DFU_FLASH_MAIN:
            l.push_back("flash");
            l.push_back("--suppress-bootloader-mem");
            l.push_back(file);
            break;
        case Status::DFU_DISCONNECT_MAIN:
        case Status::DFU_DISCONNECT_AVRDUDE:
            l.push_back("launch");
            break;
        default:
            break;
    }
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    m_process->start(dir.filePath("dfu-programmer"), l);
}
QString Programmer::programRF(QString hexFile) {
    QFile inputFile(hexFile);
    m_tmp_hex = new QTemporaryFile(QDir::temp().filePath("XXXXXX.hex"));
    if (m_tmp_hex->open()) {
        QTextStream out(m_tmp_hex);
        if (inputFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&inputFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                // DEADBEEF is a placeholder for each rx id, and the there is a checksum of 0D by default. Replace both, strip the checksum then generate a new checksum
                // Also, flip the endianness, as QString::number is printing big endian
                if (line.contains("EFBEADDEEFBEADDE")) {
                    uint32_t tx = m_parent_device->getRFID();
                    uint32_t rx = m_parent_device->generateClientRFID();
                    QString txStr = QString::number(qToBigEndian(tx), 16);
                    QString rxStr = QString::number(qToBigEndian(rx), 16);
                    // Zero pad to the right, because endianness
                    rxStr += QString("0").repeated(8 - rxStr.length());
                    txStr += QString("0").repeated(8 - txStr.length());
                    line = line.replace("EFBEADDEEFBEADDE", txStr + rxStr).toUpper().chopped(2);
                    uint16_t checksum = 0;
                    for (int i = 1; i < line.length(); i += 2) {
                        checksum += line.mid(i, 2).toInt(NULL, 16);
                    }
                    checksum = ((~checksum) & 0xff) + 1;
                    line += QString::number(checksum, 16).toUpper();
                }
                out << line << Qt::endl;
            }
            inputFile.close();
        }
        m_tmp_hex->close();
    }
    return m_tmp_hex->fileName();
}
void Programmer::programAvrDude() {
    m_status = Status::AVRDUDE;
    statusChanged(m_status);
    board_t board = m_device->getBoard();
    QString hexFile = "ardwiino-" + board.hexFile + "-" + board.processor + "-" + QString::number(board.cpuFrequency) + (m_restore ? "-restore" : "") + (m_rf ? "-rf" : "") + ".hex";
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("firmware");
    m_process_out.clear();
    QString file = dir.filePath(hexFile);
    if (m_rf) {
        file = programRF(file);
    }
    dir.cdUp();
    dir.cd("binaries");
    dir.cd("avrdude");
    auto confDir = QDir(dir);
    confDir.cd("etc");
    dir.cd("bin");
    m_process = new QProcess();
    m_process->setWorkingDirectory(dir.path());
    QStringList l = {
        "-C",
        confDir.filePath("avrdude.conf"),
        "-c",
        board.protocol,
        "-p",
        board.processor,
        "-b",
        QString::number(board.baudRate),
        "-P",
        static_cast<SerialDevice *>(m_device)->getLocation(),
        "-U",
        "flash:w:" + file + ":a",
    };
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    m_process->start(dir.filePath("avrdude"), l);
}
auto Programmer::program(Device *port) -> bool {
    m_count = 0;
    m_device = port;
    if (m_status == Status::WAIT) {
        if (m_device->getBoard().protocol == "pico") {
            port->bootloader();
            QTimer::singleShot(100, [&]() {
                programPico();
                m_status = Status::DISCONNECT_PICOBOOT;
                emit statusChanged(m_status);
                emit statusVChanged(getStatusDescription());
            });
            m_status = Status::PICOBOOT;
        } else if (m_device->getBoard().hasDFU) {
            if (m_restore) {
                m_steps = 1;
            } else {
                m_steps = 6;
            }
            DfuArduino *dfu = dynamic_cast<DfuArduino *>(m_device);
            if (dfu) {
                m_status = Status::DFU_ERASE_AVRDUDE;
                programDFU();
            } else {
                port->bootloader();
                m_status = Status::DFU_CONNECT_AVRDUDE;
            }
        } else {
            m_steps = 1;
            if (port->getBoard().inBootloader) {
                programAvrDude();
            } else {
                port->bootloader();
                m_status = Status::WAIT_AVRDUDE;
            }
        }
    }

    emit statusChanged(m_status);
    emit statusVChanged(getStatusDescription());
    return false;
}

void Programmer::complete(int exitCode, QProcess::ExitStatus exitStatus) {
    (void)exitStatus;
    QObject::disconnect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::disconnect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::disconnect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    QObject::disconnect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    m_count = 0;
    m_step++;
    switch (m_status) {
        case Status::DFU_ERASE_AVRDUDE:
            m_status = Status::DFU_FLASH_AVRDUDE;
            programDFU();
            break;
        case Status::DFU_FLASH_AVRDUDE:
            m_status = Status::DFU_DISCONNECT_AVRDUDE;
            programDFU();
            break;
        case Status::DFU_ERASE_MAIN:
            m_status = Status::DFU_FLASH_MAIN;
            programDFU();
            break;
        case Status::DFU_FLASH_MAIN:
            m_status = Status::DFU_DISCONNECT_MAIN;
            programDFU();
            break;
        case Status::AVRDUDE:
            if (m_device->getBoard().hasDFU) {
                m_status = Status::DFU_CONNECT_MAIN;
            } else if (m_device->getBoard().shortName == "mini") {
                // Minis dont't ever need to get reconnected.
                m_status = Status::COMPLETE;
                m_rf = false;
                m_device = m_parent_device;
                m_tmp_hex = NULL;
                m_parent_device->writeConfig();
                m_parent_device = NULL;
                break;
            } else {
                m_status = Status::DISCONNECT_AVRDUDE;
            }
            if (!m_rf) {
                m_device->bootloader();
                m_device->bootloader();
            } else {
                m_tmp_hex = NULL;
            }
            break;
    }
    emit statusChanged(m_status);
    emit statusVChanged(getStatusDescription());
}

void Programmer::onReady() {
    QString out = m_process->readAllStandardOutput();
    QString outErr = m_process->readAllStandardError();
    m_process_out += outErr;
    m_process_out += out;
    if (outErr.contains('#')) {
        m_count += outErr.count('#');
        //For Avrdude, each # counts for 2%, and there are 3 steps. (read flags, write flash, verify flash)
        setPercentage(m_count, 50 * 3, m_step, m_steps);
    } else if (outErr.contains('>')) {
        m_count += outErr.count('>');
        //For Dfu Programmer, there are a total of 32 >'s per line, and there are two steps (write flash, verify flash)
        setPercentage(m_count, 32 * 2, m_step, m_steps);
    }
    setPercentage(m_process_percent);
    emit processOutChanged(m_process_out);
}
