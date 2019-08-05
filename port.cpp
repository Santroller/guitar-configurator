#include "port.h"
#include <QDebug>

Port::Port(const QSerialPortInfo *serialPortInfo, QObject *parent) : QObject(parent), m_serialPort(serialPortInfo)
{
    m_description = serialPortInfo->portName();
    if (isArdwiino()) {
        m_description = "Ardwiino - "+m_description;
    }
    emit descriptionChanged(m_description);
}


bool Port::isArdwiino() {
    return m_serialPort->vendorIdentifier() == 0x1209 && m_serialPort->productIdentifier() == 0x2882;
}
