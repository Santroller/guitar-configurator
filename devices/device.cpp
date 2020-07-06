#include "device.h"
Device::Device(UsbDevice_t deviceID, QObject* parent): QObject(parent), m_deviceID(deviceID) {

}
void Device::setBoardType(QString board) {
    m_board = ArdwiinoLookup::findByBoard(board);
}