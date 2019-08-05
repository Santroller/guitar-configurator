#include "portscanner.h"
#include <QDebug>

PortScanner::PortScanner(QObject *parent) : QObject(parent)
{
        checkPorts();
}
void PortScanner::checkPorts() {
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    m_model.clear();
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
            m_model.push_back(new Port(&serialPortInfo));
    }
    emit modelChanged(m_model);
}
