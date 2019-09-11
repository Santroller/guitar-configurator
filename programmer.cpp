#include "programmer.h"
#include <QDebug>
#include <QProcess>
#include <QStringList>
#include <QCoreApplication>
#include <QDir>
#include <QRegularExpression>
#include <ardwiinolookup.h>

Programmer::Programmer(QObject *parent) : QObject(parent), m_status(Status::WAIT_AVRDUDE)
{

}
void Programmer::programDFU() {
    board_t board = ArdwiinoLookup::retriveDFUVariant(m_port->getBoard());
    QString hexFile = "ardwiino-" + board.hexFile + "-"+board.processor+"-"+QString::number(board.cpuFrequency);
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
    case Status::DFU_EEPROM:
        l.push_back("flash");
        l.push_back("--eeprom");
        l.push_back(file+".eep");
        break;
    case Status::DFU_FLASH:
        l.push_back("flash");
        l.push_back(file+".hex");
        break;
    default:
        break;
    }
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::connect(m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    m_port->prepareUpload();
    m_process->start(dir.filePath("dfu-programmer"), l);
    //Autodetect when a user has correctly programmed dfu.
    //When dfu programming is complete, ask the user to unplug-replug
}

void Programmer::programAvrDude() {
    m_status = Status::AVRDUDE;
    statusChanged(m_status);
    board_t board = m_port->getBoard();
    QString hexFile = "ardwiino-" + board.hexFile + "-"+board.processor+"-"+QString::number(board.cpuFrequency);
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
        "-U", "flash:w:"+file+".hex:a",
        "-U", "eeprom:w:"+file+".eep:a"
    };
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::connect(m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    m_port->prepareUpload();
    m_process->start(dir.filePath("avrdude"), l);
}
void Programmer::program(Port* port) {
    m_port = port;
    if (m_status == Status::WAIT_AVRDUDE) {
        programAvrDude();
    }
    if (m_status == Status::DFU_DISCONNECT) {
        m_status = Status::COMPLETE;
    }
    qDebug() << m_status;
    statusChanged(m_status);
}

void Programmer::complete(int exitCode, QProcess::ExitStatus exitStatus) {
    QObject::disconnect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::disconnect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::disconnect(m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    QObject::disconnect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));

    qDebug() << exitStatus;
    switch (m_status) {
    case Status::AVRDUDE:
        if (ArdwiinoLookup::hasDFUVariant(m_port->getBoard())) {
            m_status = Status::DFU_CONNECT;
            programDFU();
        } else {
            m_status = Status::COMPLETE;
        }
        break;
    case Status::DFU_CONNECT:
        if (exitCode == 0) {
            m_status = Status::DFU_ERASE;
        }
        programDFU();
        break;
    case Status::DFU_ERASE:
        m_status = Status::DFU_EEPROM;
        programDFU();
        break;
    case Status::DFU_EEPROM:
        m_status = Status::DFU_FLASH;
        programDFU();
        break;
    case Status::DFU_FLASH:
        m_status = Status::DFU_DISCONNECT;
        break;
    default:
        break;

    }
    statusChanged(m_status);
}

void Programmer::onReady() {
    QString out = m_process->readAllStandardOutput();
    QString out2 = m_process->readAllStandardError();
    m_process_out += out2;
    m_process_out += out;
    bool hasDfu = ArdwiinoLookup::hasDFUVariant(m_port->getBoard());
    //Each # counts for 2%, and there are 5 steps, so 500% total. 2/500 rescales that back to 100%.
    m_process_percent += out2.count('#')*((100.0/50.0)/500.0) * (hasDfu?0.5:1);
    m_process_percent += out2.count('>')*((100.0/32.0)/800.0) * hasDfu;
    emit processOutChanged(m_process_out);
    emit processPercentChanged(m_process_percent);
}
