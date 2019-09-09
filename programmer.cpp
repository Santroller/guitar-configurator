#include "programmer.h"
#include <QDebug>
#include <QProcess>
#include <QCoreApplication>
#include <QDir>
Programmer::Programmer(QObject *parent) : QObject(parent)
{

}

void Programmer::execute() {
    m_process_out.clear();
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("binaries");
    dir.cd("avrdude");
    dir.cd("bin");
    m_process = new QProcess();
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    m_process->start(dir.filePath("avrdude"));
}

void Programmer::onReady() {
    m_process_out += m_process->readAllStandardError();
    m_process_out += m_process->readAllStandardOutput();
}
