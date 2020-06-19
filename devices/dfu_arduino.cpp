#include "dfu_arduino.h"
DfuArduino::DfuArduino(QString processor, QObject* parent) : Device(parent), m_processor(processor) {
}
QString DfuArduino::getDescription() {
    return "Arduino Uno / Mega / Mega ADK in DFU mode detected";
}
bool DfuArduino::isReady() {
    return false;
}
void DfuArduino::close() {
}
bool DfuArduino::open() {
    return true;
}
void DfuArduino::bootloader() {
    
}