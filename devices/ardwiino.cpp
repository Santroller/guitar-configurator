#include "ardwiino.h"

#include <QSettings>
#include <QThread>
#include <Windows.h>
#include <winusb.h>
#include <fileapi.h>
#define REQ_HID_GET_REPORT 1
#define REQ_HID_SET_REPORT 9
Ardwiino::Ardwiino(UsbDevice_t devt, QObject* parent) : Device(devt, parent), m_hiddev(nullptr), m_configurable(false) {
}
bool Ardwiino::open() {
    if (m_deviceID.serial.isEmpty()) return false;

    //TODO: Cool, we can read and write. to
    //TODO: we should probably abstract this stuff away to usb/winusb.cpp, add a function that can just read and a function that just writes and just reads, then make a libusb version.
    //TODO: However that confirms that we can just get rid of hidapi now!
    WINUSB_INTERFACE_HANDLE WinusbHandle;
    HANDLE                  DeviceHandle = CreateFile(m_deviceID.hidPath.toUtf8(),
                                              GENERIC_WRITE | GENERIC_READ,
                                              FILE_SHARE_WRITE | FILE_SHARE_READ,
                                              NULL,
                                              OPEN_EXISTING,
                                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                              NULL);
    if(!WinUsb_Initialize(DeviceHandle,  &WinusbHandle)) {
        qDebug() << "Winusb Failure!";
        return false;
    }
    BOOL bResult = TRUE;
    UCHAR d[64];
    WINUSB_SETUP_PACKET SetupPacket;
    ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
    ULONG cbSent = 0;

    //Create the setup packet
    //READ
    BM_REQUEST_TYPE rt;
    rt.s.Dir = BMREQUEST_DEVICE_TO_HOST;
    rt.s.Reserved = 0;
    rt.s.Type = BMREQUEST_CLASS;
    rt.s.Recipient = BMREQUEST_TO_INTERFACE;
    SetupPacket.RequestType = rt.B;
    SetupPacket.Request = REQ_HID_GET_REPORT;

    //WRITE

    /*BM_REQUEST_TYPE rt;
    rt.s.Dir = BMREQUEST_DEVICE_TO_HOST;
    rt.s.Reserved = 0;
    rt.s.Type = BMREQUEST_CLASS;
    rt.s.Recipient = BMREQUEST_TO_INTERFACE;
    SetupPacket.RequestType = rt.B;
    SetupPacket.Request = REQ_HID_SET_REPORT;*/
    SetupPacket.Value = 0;
    SetupPacket.Index = 0;
    SetupPacket.Length = sizeof(d) * sizeof(UCHAR);

    bResult = WinUsb_ControlTransfer(WinusbHandle, SetupPacket, d, sizeof(d) * sizeof(UCHAR), &cbSent, 0);
    if (!bResult)
    {
       auto err = GetLastError();
       qDebug() << "Unable to send" << err;
    } else {
       qDebug() << QString::fromUtf8((char*)d,64);
    }

    qDebug() << "Data sent: " <<  sizeof(d) << " Actual data transferred: " << cbSent;
    if (bResult) {
        data_t data = *((data_t*)d);
        Configuration_t conf;
//        while (data.offset != 0 || !data.board[0]) {
//            data = readData();
//        }
        
        m_board = ArdwiinoLookup::findByBoard(QString::fromUtf8((char*)data.board), false);
        m_board.cpuFrequency = data.cpu_freq;
        m_extension = data.extension;
        qDebug() << m_board.name;
       // while (true) {
         //   if (data.board[0]) {
                qDebug() << sizeof(Configuration_t) << data.offset << "->" << qMin(sizeof(data.data), sizeof(Configuration_t) - data.offset);
                memcpy(((uint8_t*)&conf) + data.offset, data.data, qMin(sizeof(data.data), sizeof(Configuration_t) - data.offset));
//                if (data.offset + sizeof(data.data) >= sizeof(conf)) {
//                    break;
//                }
            /*
            data = readData();
        }*/
        m_configuration = new DeviceConfiguration(conf);
        m_configurable = !ArdwiinoLookup::isOutdatedArdwiino(m_deviceID.releaseNumber);
        // m_configurable = true;
        emit configurationChanged();
        emit configurableChanged();
        emit boardImageChanged();
    } else {
        // TODO: handle errors (Atleast tell the user that some undetected device was found)
        qDebug() << "UNABLE TO OPEN";
    }
    return bResult;
}
data_t Ardwiino::readData() {
    QByteArray data(sizeof(data_t) + 1, '\0');
    data[0] = 0;
    hid_get_feature_report(m_hiddev, reinterpret_cast<unsigned char*>(data.data()), data.size());
    data.remove(0, 1);
    auto err = hid_error(m_hiddev);
    if (err) {
        // TODO: handle errors (Tell the user that we could not communicate with the controller)
        qDebug() << QString::fromWCharArray(err);
    }
    return *(data_t*)data.data();
}
#define PACKET_SIZE 64
void Ardwiino::writeData(int cmd, QByteArray dataToSend) {
    QByteArray data(PACKET_SIZE, '\0');
    data[0] = 0;
    data[1] = cmd;
    for (int i = 0; i < dataToSend.length(); i++) {
        data[i + 2] = dataToSend[i];
    }
    hid_send_feature_report(m_hiddev, reinterpret_cast<unsigned char*>(data.data()), data.size());
    auto err = hid_error(m_hiddev);
    if (err) {
        // TODO: handle errors (Tell the user that we could not communicate with the controller)
        qDebug() << "error writing" << cmd << QString::fromWCharArray(err);
    }
}
// Reserve space for the report id, command and the offset.
#define PARTIAL_CONFIG_SIZE PACKET_SIZE - 3
void Ardwiino::writeConfig() {
    auto config = m_configuration->getConfig();
    auto configCh = reinterpret_cast<char*>(&config);
    uint offset = 0;
    QByteArray data;
    while (offset < sizeof(Configuration_t)) {
        data.clear();
        data.push_back(offset);
        data.push_back(QByteArray::fromRawData(configCh + offset, PARTIAL_CONFIG_SIZE));
        writeData(COMMAND_WRITE_CONFIG, data);
        offset += PARTIAL_CONFIG_SIZE;
        QThread::currentThread()->msleep(100);
    }
    uint8_t st = config.main.subType;
    if (m_configuration->isDrum()) {
        st = REAL_DRUM_SUBTYPE;
    } else if (m_configuration->isGuitar()) {
        st = REAL_GUITAR_SUBTYPE;
    }
    writeData(COMMAND_WRITE_SUBTYPE, QByteArray(1, config.main.subType));
    QThread::currentThread()->msleep(100);
    writeData(COMMAND_REBOOT);
    m_hiddev = NULL;
}
void Ardwiino::findDigital(QJSValue callback) {
    m_pinDetectionCallback = callback;
    QTimer::singleShot(100, [&]() {
        uint8_t pin = readData().detectedPin;
        if (pin == 0xFF) {
            findDigital(m_pinDetectionCallback);
        } else {
            QJSValueList args;
            args << QJSValue(pin);
            m_pinDetectionCallback.call(args);
            qDebug() << pin;
        }
    });
}
void Ardwiino::findAnalog(QJSValue callback) {
    m_pinDetectionCallback = callback;
    QTimer::singleShot(100, [&]() {
        uint8_t pin = readData().detectedPin;
        if (pin == 0xFF) {
            findAnalog(m_pinDetectionCallback);
        } else {
            QJSValueList args;
            args << QJSValue(pin);
            m_pinDetectionCallback.call(args);
        }
    });
}
QString Ardwiino::getDescription() {
    if (!isReady()) {
        return "Ardwiino - Unable to communicate";
    }
    QString desc = "Ardwiino - " + m_board.name + " - " + ArdwiinoDefines::getName(m_configuration->getMainSubType());
    if (m_configuration->getMainInputType() == ArdwiinoDefines::WII) {
        auto extName = ArdwiinoDefines::getName((ArdwiinoDefines::WiiExtType)m_extension);
        if (extName == "Unknown") {
            extName = "Wii Unknown Extension";
        }
        desc += " - " + extName;
    } else if (m_configuration->getMainInputType() == ArdwiinoDefines::PS2) {
        auto extName = ArdwiinoDefines::getName((ArdwiinoDefines::PsxControllerType)m_extension);
        desc += " - " + extName;
    } else {
        desc += " - " + ArdwiinoDefines::getName(m_configuration->getMainInputType());
    }
    // On windows, we can actually write the description to registry in a way that applications will pick it up.
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\System\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_1209&PID_2882", QSettings::NativeFormat);
    settings.setValue("OEMName", desc);
#endif
    return desc;
}
bool Ardwiino::isReady() {
    return m_hiddev;
}
void Ardwiino::close() {
    if (m_hiddev) {
        hid_close(m_hiddev);
        m_hiddev = NULL;
    }
}
void Ardwiino::bootloader() {
    writeData(COMMAND_JUMP_BOOTLOADER, {});
}
