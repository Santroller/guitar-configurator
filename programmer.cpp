#include "programmer.h"

#include <ardwiinolookup.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QRegularExpression>
#include <QStringList>

Programmer::Programmer(QObject* parent) : QObject(parent), m_status(Status::NOT_PROGRAMMING), m_port(nullptr), m_restore(false) {
}
// Using these will allow us to simplify a lot of the waiting code, espically waiting for dfu. For example, detectBoard is now not necessary at all!
void Programmer::dfuDeviceAdded(DfuArduino* device) {

}
void Programmer::dfuDeviceUnplugged(DfuArduino* device) {
    
}
void Programmer::ardwiinoDeviceAdded(Ardwiino* device) {
    
}
void Programmer::ardwiinoDeviceUnplugged(Ardwiino* device) {
    
}
void Programmer::arduinoDeviceAdded(Arduino* device) {
    
}
void Programmer::arduinoDeviceUnplugged(Arduino* device) {
    
}
auto Programmer::detectBoard() -> board_t {
    // auto dir = QDir(QCoreApplication::applicationDirPath());
    // dir.cd("binaries");
    // for (board_t board : ArdwiinoLookup::boards) {
    //     if (!board.shortName.startsWith(m_port->board().shortName)) continue;
    //     m_process = new QProcess();
    //     m_process->setWorkingDirectory(dir.path());
    //     connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    //     m_process->start(dir.filePath("dfu-programmer"), {board.processor, "get"});
    //     m_process->waitForFinished();
    //     if (m_process->exitCode() == 0) {
    //         return board;
    //     }
    // }
    // return {};
    return {};
}
void Programmer::programDFU() {
    // board_t board = detectBoard();
    // QString hexFile = "ardwiino-" + board.hexFile + "-" + board.processor + "-" + QString::number(board.cpuFrequency);
    // if (m_restore) {
    //     hexFile += "-restore";
    // }
    // if (m_status == Status::DFU_FLASH_AVRDUDE) {
    //     hexFile += "-usbserial";
    // }
    // hexFile += ".hex";
    // auto dir = QDir(QCoreApplication::applicationDirPath());
    // dir.cd("firmware");
    // m_process_out.clear();
    // QString file = dir.filePath(hexFile);
    // dir.cdUp();
    // dir.cd("binaries");
    // m_process = new QProcess();
    // m_process->setWorkingDirectory(dir.path());
    // QStringList l = {
    //     board.processor};
    // switch (m_status) {
    //     case Status::DFU_CONNECT_MAIN:
    //     case Status::DFU_CONNECT_AVRDUDE:
    //         l.push_back("get");
    //         break;
    //     case Status::DFU_ERASE_MAIN:
    //     case Status::DFU_ERASE_AVRDUDE:
    //         l.push_back("erase");
    //         break;
    //     case Status::DFU_FLASH_AVRDUDE:
    //     case Status::DFU_FLASH_MAIN:
    //         l.push_back("flash");
    //         l.push_back("--suppress-bootloader-mem");
    //         l.push_back(file);
    //         break;
    //     case Status::DFU_DISCONNECT_MAIN:
    //     case Status::DFU_DISCONNECT_AVRDUDE:
    //         l.push_back("launch");
    //         break;
    //     default:
    //         break;
    // }
    // QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    // QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    // QObject::connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    // connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    // m_process->start(dir.filePath("dfu-programmer"), l);
}

void Programmer::programAvrDude() {
    // m_status = Status::AVRDUDE;
    // statusChanged(m_status);
    // board_t board = m_port->getBoard();
    // QString hexFile = "ardwiino-" + board.hexFile + "-" + board.processor + "-" + QString::number(board.cpuFrequency) + ".hex";
    // auto dir = QDir(QCoreApplication::applicationDirPath());
    // dir.cd("firmware");
    // m_process_out.clear();
    // QString file = dir.filePath(hexFile);
    // dir.cdUp();
    // dir.cd("binaries");
    // dir.cd("avrdude");
    // auto confDir = QDir(dir);
    // confDir.cd("etc");
    // dir.cd("bin");
    // m_process = new QProcess();
    // m_process->setWorkingDirectory(dir.path());
    // QStringList l = {
    //     "-C",
    //     confDir.filePath("avrdude.conf"),
    //     "-c",
    //     board.protocol,
    //     "-p",
    //     board.processor,
    //     "-P",
    //     m_port->getPort(),
    //     "-U",
    //     "flash:w:" + file + ":a",
    // };
    // QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    // QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    // QObject::connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    // connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    // m_process->start(dir.filePath("avrdude"), l);
}
auto Programmer::program(Device* port) -> bool {
    // if (m_status == Status::NOT_PROGRAMMING) return false;
    // bool ret = false;
    // m_port = port;
    // if (m_status == Status::WAIT) {
    //     if (m_port->isArdwiino()) {
    //         m_port->jump();
    //     }
    //     if (m_port->getBoard().hasDFU && m_port->isAlreadyDFU()) {
    //         m_port->setBoard(m_port->getBoard().shortName.replace("-" + m_port->getBoard().processor, ""), 0);
    //         m_status = Status::DFU_CONNECT_AVRDUDE;
    //         programDFU();
    //         return true;
    //     }
    //     if (m_port->isAlreadyDFU()) {
    //         programAvrDude();
    //         return true;
    //     }
    //     if (m_restore) {
    //         m_status = Status::DFU_CONNECT_AVRDUDE;
    //         m_port->jumpUNO();
    //         programDFU();
    //     } else {
    //         if (m_port->getBoard().hasDFU) {
    //             programAvrDude();
    //         } else {
    //             m_port->jump();
    //             m_port->prepareUpload();
    //             m_status = Status::DFU_CONNECT_AVRDUDE;
    //             ret = true;
    //         }
    //     }
    // } else if (m_status == Status::DFU_CONNECT_AVRDUDE) {
    //     if (!m_port->getBoard().hasDFU) {
    //         programAvrDude();
    //     }
    // } else if (m_status == Status::DFU_DISCONNECT_AVRDUDE) {
    //     if (m_port->getBoard().hasDFU && m_port->isAlreadyDFU()) {
    //         m_status = Status::AVRDUDE;
    //         programAvrDude();
    //     } else {
    //         m_status = Status::COMPLETE;
    //         ret = true;
    //     }
    // }
    // emit statusChanged(m_status);
    // emit statusVChanged(getStatusDescription());
    // return ret;
    return false;
}

void Programmer::complete(int exitCode, QProcess::ExitStatus exitStatus) {
    (void)exitStatus;
    // QObject::disconnect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    // QObject::disconnect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    // QObject::disconnect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    // QObject::disconnect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));

    // switch (m_status) {
    //     case Status::AVRDUDE:
    //         if (exitCode == 0) {
    //             if (m_port->getBoard().hasDFU && m_port->isAlreadyDFU()) {
    //                 m_status = Status::COMPLETE;
    //                 m_port->scanAfterDFU();
    //             } else if (m_port->getBoard().hasDFU) {
    //                 m_status = Status::DFU_CONNECT_AVRDUDE;
    //                 m_port->jumpUNO();
    //                 programDFU();
    //             } else {
    //                 m_status = Status::DFU_DISCONNECT_AVRDUDE;
    //             }
    //         }
    //         break;
    //     case Status::DFU_CONNECT_AVRDUDE:
    //         if (exitCode == 0) {
    //             m_status = Status::DFU_ERASE_AVRDUDE;
    //         }
    //         programDFU();
    //         break;
    //     case Status::DFU_ERASE_AVRDUDE:
    //         m_status = Status::DFU_FLASH_AVRDUDE;
    //         programDFU();
    //         break;
    //     case Status::DFU_FLASH_AVRDUDE:
    //         m_port->prepareRescan();
    //         m_status = Status::DFU_DISCONNECT_AVRDUDE;
    //         programDFU();
    //         break;
    //     default:
    //         break;
    // }
    // emit statusChanged(m_status);
    // emit statusVChanged(getStatusDescription());
}

void Programmer::onReady() {
    // QString out = m_process->readAllStandardOutput();
    // QString out2 = m_process->readAllStandardError();
    // m_process_out += out2;
    // m_process_out += out;
    // if (m_restore) {
    //     m_process_percent += out2.count('>') * ((100.0 / 32.0) / 200.0);
    // } else {
    //     bool hasDfu = m_port->getBoard().hasDFU;
    //     //Each # counts for 2%, and there are 3 steps, so 300% total. 2/300 rescales that back to 100%.
    //     m_process_percent += out2.count('#') * ((100.0 / 50.0) / 300.0) * (hasDfu ? 0.5 : 1);
    //     m_process_percent += out2.count('>') * ((100.0 / 32.0) / 400.0) * hasDfu;
    // }
    // emit processOutChanged(m_process_out);
    // emit processPercentChanged(m_process_percent);
    // emit statusVChanged(getStatusDescription());
}
