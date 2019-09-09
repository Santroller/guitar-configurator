#include "programmer.h"
#include <QDebug>
#include <QProcess>
#include <QStringList>
#include <QCoreApplication>
#include <QDir>

Programmer::Programmer(QObject *parent) : QObject(parent)
{

}

void Programmer::program(Port* port) {
    board_t board = port->getBoard();
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
        "-P", port->getPort(),
        "-U", "flash:w:"+file+":a"
    };
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    port->prepareUpload();
    m_process->start(dir.filePath("avrdude"), l);
}

void Programmer::onReady() {
    m_process_out += m_process->readAllStandardError();
    m_process_out += m_process->readAllStandardOutput();
    emit processOutChanged(m_process_out);
}
