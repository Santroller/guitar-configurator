#include "null_device.h"
NullDevice::NullDevice(QObject* parent): Device({}, parent) {

}
QString NullDevice::getDescription() {
    return "No device detected";
}
bool NullDevice::isReady() {
    return false;
}
void NullDevice::close() {
    
}
bool NullDevice::open() {
    return true;
}
void NullDevice::bootloader() {
    
}