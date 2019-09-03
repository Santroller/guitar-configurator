#include "programmer.h"
#include <QDebug>
#include <QProcess>
#include <QCoreApplication>
#include <QDir>
Programmer::Programmer(QObject *parent) : QObject(parent)
{
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("binaries");
    dir.cd("linux-64");
    dir.cd("avrdude");
    auto binDir = QDir(dir);
    binDir.cd("bin");
    m_process = new QProcess();
    auto qpe = new QProcessEnvironment(QProcessEnvironment::systemEnvironment());
    qpe->insert("PATH", qpe->value("PATH")+":"+dir.filePath("lib"));
    m_process->setProcessEnvironment(*qpe);
    m_process->setWorkingDirectory(binDir.path());
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    m_process->start("avrdude");
    //TODO: Grab avrdude from here: https://github.com/arduino/Arduino/tree/master/build``
}

void Programmer::onReady() {
    m_process_out += m_process->readAllStandardError();
    m_process_out += m_process->readAllStandardOutput();
}
