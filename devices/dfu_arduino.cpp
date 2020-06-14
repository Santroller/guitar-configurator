#include "dfu_arduino.h"
DfuArduino::DfuArduino(QString processor, QUsbDevice::Id usbId, QObject* parent) : Device(parent), m_processor(processor), m_usbId(usbId) {
}
QString DfuArduino::getDescription() {
    return "Arduino Uno / Mega / Mega ADK in DFU mode detected";
}
bool DfuArduino::isReady() {
    return false;
}
void DfuArduino::close() {
}
void DfuArduino::open() {
}