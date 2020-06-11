#pragma once
#include <math.h>

#include <QDebug>
#include <QJSValue>
#include <QObject>
#include <QQueue>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "ardwiino_defines.h"
#include "ardwiinolookup.h"
#include "submodules/Ardwiino/src/shared/output/serial_commands.h"
class OutdatedArdwiino : public QObject {
    Q_OBJECT
    explicit OutdatedArdwiino(const QSerialPortInfo &serialPortInfo, QObject *parent = nullptr); 
};