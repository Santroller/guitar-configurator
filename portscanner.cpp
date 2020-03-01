#include "portscanner.h"
#include <QDebug>

PortScanner::PortScanner(Programmer *programmer, QObject *parent) : QObject(parent), m_selected(nullptr), programmer(programmer)
{
    m_model.push_back(new Port());
}
void PortScanner::addPort(const QSerialPortInfo& serialPortInfo) {
    if (m_selected != nullptr) {
        m_selected->handleConnection(serialPortInfo);
        programmer->program(m_selected);
        if (!programmer->getRestore()) {
            m_selected->handleConnection(serialPortInfo);
            return;
        }
    }
    auto port = new Port(serialPortInfo);
    if (port->getPort() == nullptr) return;
    m_model.erase(std::remove_if(m_model.begin(), m_model.end(), [](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == "searching";}),m_model.end());
    m_model.push_back(port);
    port->open(serialPortInfo);
    emit modelChanged(m_model);
}
void PortScanner::removePort(const QSerialPortInfo& serialPortInfo) {
    if (m_selected != nullptr) {
        //Pass through to selected, replace its scanning implementation
    }
    m_model.erase(std::remove_if(m_model.begin(), m_model.end(), [serialPortInfo](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == serialPortInfo.systemLocation();}), m_model.end());
    if (m_model.length() == 0) {
        m_model.push_back(new Port());
    }
    emit modelChanged(m_model);
}
