#include "device.h"
Device::Device(QObject* parent): QObject(parent) {

}
void Device::setBoardType(QString board) {
    m_board = ArdwiinoLookup::findByBoard(board);
}