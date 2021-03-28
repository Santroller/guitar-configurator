#include "device.h"
#include "ardwiinolookup.h"
Device::Device(UsbDevice_t deviceID, QObject* parent): QObject(parent), m_deviceID(deviceID) {

}
void Device::setBoardType(QString board) {
    m_board = ArdwiinoLookup::findByBoard(board, m_board.inBootloader);
    emit boardImageChanged();
    emit boardNameChanged();
}
void Device::setBoardType(QString board, uint freq) {
    qDebug() << board << freq;
    m_board = ArdwiinoLookup::findByBoard(board, m_board.inBootloader);
    m_board.cpuFrequency = freq;
    emit boardImageChanged();
    emit boardNameChanged();
    emit boardFreqChanged();
}
