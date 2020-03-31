#include "programmer.h"
#include <QDebug>
#include <QProcess>
#include <QStringList>
#include <QCoreApplication>
#include <QDir>
#include <QRegularExpression>
#include <ardwiinolookup.h>

Programmer::Programmer(QObject *parent) : QObject(parent), m_status(Status::NOT_PROGRAMMING), m_port(nullptr), m_restore(false)
{

}

auto Programmer::detectBoard() -> board_t {
    if (!m_port->board().hasDFU) {
        return m_port->board();
    }
    auto board = ArdwiinoLookup::boards[0];
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("binaries");
    for (board_t board: ArdwiinoLookup::boards) {
        if (board.originalFirmware == "") continue;
        m_process = new QProcess();
        m_process->setWorkingDirectory(dir.path());
        connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
        m_process->start(dir.filePath("dfu-programmer"), {board.processor, "get"});
        m_process->waitForFinished();
        if (m_process->exitCode() == 0) {
            return board;
        }
    }
    return board;
}
void Programmer::programDFU() {
    board_t board = detectBoard();
    QString hexFile = "ardwiino-" + board.hexFile + "-"+board.processor+"-"+QString::number(board.cpuFrequency)+".hex";
    if (m_restore) {
        hexFile = board.originalFirmware;
    }
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("firmware");
    m_process_out.clear();
    QString file = dir.filePath(hexFile);
    dir.cdUp();
    dir.cd("binaries");
    m_process = new QProcess();
    m_process->setWorkingDirectory(dir.path());
    QStringList l = {
        board.processor
    };
    switch (m_status) {
    case Status::DFU_CONNECT:
        l.push_back("get");
        break;
    case Status::DFU_ERASE:
        l.push_back("erase");
        break;
    case Status::DFU_FLASH:
        l.push_back("flash");
        l.push_back("--suppress-bootloader-mem");
        l.push_back(file);
        break;
    case Status::DFU_DISCONNECT:
        l.push_back("launch");
        break;
    default:
        break;
    }
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::connect(m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    m_process->start(dir.filePath("dfu-programmer"), l);
}

void Programmer::programAvrDude() {
    m_status = Status::AVRDUDE;
    statusChanged(m_status);
    board_t board = m_port->getBoard();
    QString hexFile = "ardwiino-" + board.hexFile + "-"+board.processor+"-"+QString::number(board.cpuFrequency)+".hex";
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
        "-C", confDir.filePath("avrdude.conf"),
        "-c", board.protocol,
        "-p", board.processor,
        "-P", m_port->getPort(),
        "-U", "flash:w:"+file+":a",
    };
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::connect(m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    m_process->start(dir.filePath("avrdude"), l);
}
auto Programmer::program(Port* port) -> bool {
    if (m_status == Status::NOT_PROGRAMMING) return false;
    bool ret = false;
    m_port = port;
    if (m_status == Status::WAIT) {
        if (m_restore) {
            m_status = Status::DFU_CONNECT;
            m_port->jumpUNO();
            programDFU();
        } else {
            if (m_port->getBoard().hasDFU) {
                programAvrDude();
            } else {
                m_port->jump();
                m_port->prepareUpload();
                m_status = Status::DFU_CONNECT;
                ret = true;
            }
        }
    } else if (m_status == Status::DFU_CONNECT) {
        if (!m_port->getBoard().hasDFU) {
            programAvrDude();
        }
    } else if (m_status == Status::DFU_DISCONNECT) {
        m_status = Status::COMPLETE;
        ret = true;
    }
    emit statusChanged(m_status);
    emit statusVChanged(getStatusDescription());
    return ret;
}

void Programmer::complete(int exitCode, QProcess::ExitStatus exitStatus) {
    (void)exitStatus;
    QObject::disconnect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::disconnect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::disconnect(m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    QObject::disconnect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));

    switch (m_status) {
    case Status::AVRDUDE:
        if (exitCode == 0) {
            if (m_port->getBoard().hasDFU) {
                m_status = Status::DFU_CONNECT;
                programDFU();
            } else {
                m_status = Status::DFU_DISCONNECT;
            }
        }
        break;
    case Status::DFU_CONNECT:
        if (exitCode == 0) {
            m_status = Status::DFU_ERASE;
        }
        programDFU();
        break;
    case Status::DFU_ERASE:
        m_status = Status::DFU_FLASH;
        programDFU();
        break;
    case Status::DFU_FLASH:
        m_port->prepareRescan();
        m_status = Status::DFU_DISCONNECT;
        programDFU();
        break;
    default:
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
        m_process_percent += out2.count('>')*((100.0/32.0)/200.0);
    } else {
        bool hasDfu = m_port->getBoard().hasDFU;
        //Each # counts for 2%, and there are 3 steps, so 300% total. 2/300 rescales that back to 100%.
        m_process_percent += out2.count('#')*((100.0/50.0)/300.0) * (hasDfu?0.5:1);
        m_process_percent += out2.count('>')*((100.0/32.0)/400.0) * hasDfu;
    }
    emit processOutChanged(m_process_out);
    emit processPercentChanged(m_process_percent);
    emit statusVChanged(getStatusDescription());
}
