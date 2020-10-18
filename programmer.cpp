#include "programmer.h"

#include <ardwiinolookup.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QRegularExpression>
#include <QStringList>
#include "devices/serialdevice.h"
Programmer::Programmer(QObject* parent) : QObject(parent), m_status(Status::NOT_PROGRAMMING), m_device(nullptr), m_restore(false) {
    

}
void Programmer::deviceAdded(DfuArduino* device) {
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
void Programmer::deviceAdded(Ardwiino* device) {
    m_device = device;
    if (m_status == Status::DFU_DISCONNECT_MAIN || m_status == Status::DISCONNECT_AVRDUDE) {
        m_status = Status::COMPLETE;
        emit statusChanged(m_status);
        emit statusVChanged(getStatusDescription());
    }
}
void Programmer::deviceAdded(Arduino* device) {
    if (m_device && !m_device->getBoard().hasDFU) {
        device->setBoardType(m_device->getBoard().shortName, m_device->getBoard().cpuFrequency);
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

void Programmer::programAvrDude() {
    m_status = Status::AVRDUDE;
    statusChanged(m_status);
    board_t board = m_device->getBoard();
    QString hexFile = "ardwiino-" + board.hexFile + "-" + board.processor + "-" + QString::number(board.cpuFrequency) + (m_restore ? "-restore" : "") + ".hex";
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("firmware");
    m_process_out.clear();
    QString file = dir.filePath(hexFile);
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
        "-P",
        static_cast<SerialDevice*>(m_device)->getLocation(),
        "-U",
        "flash:w:" + file + ":a",
    };
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    m_process->start(dir.filePath("avrdude"), l);
}
auto Programmer::program(Device* port) -> bool {
    m_device = port;
    if (m_status == Status::WAIT) {
        if (m_device->getBoard().hasDFU) {
            DfuArduino* dfu = dynamic_cast<DfuArduino*>(m_device);
            if (dfu) {
                m_status = Status::DFU_ERASE_AVRDUDE;
                programDFU();
            } else {
                port->bootloader();
                m_status = Status::DFU_CONNECT_AVRDUDE;
            }
        } else {
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
            } else {
                m_status = Status::DISCONNECT_AVRDUDE;
            }
            m_device->bootloader();
            m_device->bootloader();
            break;
    }
    emit statusChanged(m_status);
    emit statusVChanged(getStatusDescription());
}

void Programmer::onReady() {
    QString out = m_process->readAllStandardOutput();
    QString out2 = m_process->readAllStandardError();
    m_process_out += out2;
    m_process_out += out;
    if (m_restore) {
        m_process_percent += out2.count('#') * ((100.0 / 50.0) / 300.0);
        m_process_percent += out2.count('>') * ((100.0 / 32.0) / 200.0);
    } else {
        bool hasDfu = m_device->getBoard().hasDFU;
        //For avrdude, each # counts for 2%, and there are 4 steps, so 400% total. 2/400 rescales that back to 100%.
        //For dfu, there are a total of 32 >'s per line, and there are two steps.
        //There are three different processes, two dfu and one avrdude. we need 100%, so we can do two 33s and one 34.
        m_process_percent += out2.count('#') * ((100.0 / 50.0) / 300.0) * (hasDfu ? 0.34 : 1);
        m_process_percent += out2.count('>') * ((100.0 / 32.0) / 200.0) * (hasDfu ? 0.33 : 0);
    }
    emit processOutChanged(m_process_out);
    emit processPercentChanged(m_process_percent);
    emit statusVChanged(getStatusDescription());
}
