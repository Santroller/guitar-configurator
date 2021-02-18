#include "programmer.h"

#include <ardwiinolookup.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QRegularExpression>
#include <QStringList>
#include <QtEndian>

#include "devices/serialdevice.h"
#include "picoboot_connection_cxx.h"
extern "C" {
#include "arguments.h"
#include "atmel.h"
#include "commands.h"
#include "dfu-device.h"
#include "dfu.h"
int debug = 1000;
libusb_context* usbcontext = NULL;
void dfu_debug(const char* file, const char* function, const int line,
               const int level, const char* format, ...) {
    qDebug() << level;
    if (level < debug) {
        va_list va_arg;

        va_start(va_arg, format);
        QString s;
        s.sprintf("%s:%d: ", file, line);
        qDebug() << s;
        s.vsprintf(format, va_arg);
        qDebug() << s;
        va_end(va_arg);
    }
}
}
#define DFU_DEBUG_THRESHOLD 100
#define DFU_TRACE_THRESHOLD 200
#define DFU_MESSAGE_DEBUG_THRESHOLD 300
#define DEBUG(...) dfu_debug(__FILE__, __FUNCTION__, __LINE__, DFU_DEBUG_THRESHOLD, __VA_ARGS__)
Programmer::Programmer(QObject* parent) : QObject(parent), m_status(Status::NOT_PROGRAMMING), m_device(nullptr), m_restore(false), m_rf(false) {
}
void Programmer::prepareRF(Ardwiino* device) {
    m_rf = true;
    m_parent_device = device;
}
void initDFUDevice(UsbDevice_t devt, dfu_device_t* dfu_device) {
    auto device = devt.dev;
    int32_t tmp;
    DEBUG("found device at USB:%d,%d\n", libusb_get_bus_number(device), libusb_get_device_address(device));
    /* We found a device that looks like it matches...
             * let's try to find the DFU interface, open the device
             * and claim it. */

    struct libusb_device_descriptor descriptor;

    if (libusb_get_device_descriptor(device, &descriptor)) {
        DEBUG("Failed in libusb_get_device_descriptor\n");
        return;
    }
    tmp = dfu_find_interface(device, false,
                             descriptor.bNumConfigurations);

    if (0 <= tmp) { /* The interface is valid. */
        dfu_device->interface = tmp;

        if (0 == libusb_open(device, &dfu_device->handle)) {
            DEBUG("opened interface %d...\n", tmp);
            if (0 == libusb_set_configuration(dfu_device->handle, 1)) {
                DEBUG("set configuration %d...\n", 1);
                if (0 == libusb_claim_interface(dfu_device->handle, dfu_device->interface)) {
                    DEBUG("claimed interface %d...\n", dfu_device->interface);

                    switch (dfu_make_idle(dfu_device, true)) {
                        case 0:
                            DEBUG("Success.\n");
                            return;
                    }

                    DEBUG("Failed to put the device in dfuIDLE mode.\n");
                    libusb_release_interface(dfu_device->handle, dfu_device->interface);
                } else {
                    DEBUG("Failed to claim the DFU interface.\n");
                }
            } else {
                DEBUG("Failed to set configuration.\n");
            }

            libusb_close(dfu_device->handle);
        }
    }
}
void Programmer::deviceAdded(DfuArduino* device) {
    qDebug() << "device";
    auto devt = device->getUSBDevice();
    dfu_device_t dev;
    initDFUDevice(devt, &dev);
    programmer_arguments arg;
    arg.command = com_launch;
    arg.com_launch_config.noreset = false;
    arg.device_type = ADC_AVR;
    execute_command(&dev, &arg);
    libusb_release_interface(dev.handle, dev.interface);
    if (m_status != Status::NOT_PROGRAMMING) {
        QString board = m_device->getBoard().shortName;
        if (!board.contains("-")) {
            board += "-" + device->getProcessor();
        }
        device->setBoardType(board);
    }
    m_device = device;
    switch (m_status) {
        case Status::DFU_CONNECT_AVRDUDE:
            m_status = Status::DFU_ERASE_AVRDUDE;
            programDFU();
            break;
        case Status::DFU_CONNECT_MAIN:
            m_status = Status::DFU_ERASE_MAIN;
            programDFU();
            break;
    }
    emit statusChanged(m_status);
    emit statusVChanged(getStatusDescription());
}
void Programmer::deviceAdded(Ardwiino* device) {
    m_device = device;
    if (m_status == Status::DFU_DISCONNECT_MAIN || m_status == Status::DISCONNECT_AVRDUDE) {
        m_status = Status::COMPLETE;
        if (m_rf) {
            m_rf = false;
            m_device = m_parent_device;
            m_parent_device->writeConfig();
            m_parent_device = NULL;
        }
        emit statusChanged(m_status);
        emit statusVChanged(getStatusDescription());
    }
}
void Programmer::deviceAdded(Arduino* device) {
    if (m_device && !m_device->getBoard().hasDFU) {
        device->setBoardType(m_device->getBoard().shortName, m_device->getBoard().cpuFrequency);
    }
    m_device = device;
    if (m_status == Status::DFU_DISCONNECT_AVRDUDE || m_status == Status::WAIT_AVRDUDE) {
        if (m_restore && m_device->getBoard().hasDFU) {
            m_status = Status::COMPLETE;
        } else {
            m_status = Status::AVRDUDE;
            programAvrDude();
        }
    }
    if (m_restore && m_status == Status::DISCONNECT_AVRDUDE && !m_device->getBoard().hasDFU) {
        m_status = Status::COMPLETE;
    }
    emit statusChanged(m_status);
    emit statusVChanged(getStatusDescription());
}
void Programmer::programDFU() {
    board_t board = m_device->getBoard();
    QString hexFile = "ardwiino-" + board.hexFile + "-" + board.processor + "-" + QString::number(board.cpuFrequency);
    if (m_restore) {
        hexFile += "-restore";
    } else if (m_status == Status::DFU_FLASH_AVRDUDE) {
        hexFile += "-usbserial";
    }
    hexFile += ".hex";
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("firmware");
    m_process_out.clear();
    QString file = dir.filePath(hexFile);
    dir.cdUp();
    dir.cd("binaries");
    m_process = new QProcess();
    m_process->setWorkingDirectory(dir.path());
    UsbDevice_t usb = m_device->getUSBDevice();
    // Atmel devices use libusb-0.1 drivers, and these do not appear to expose a port number on windows.
#ifndef Q_OS_WIN
    QStringList l = {
        board.processor + ":" + QVariant(usb.bus).toString() + "," + QVariant(usb.port).toString()};
#else
    QStringList l = {board.processor};
#endif
    switch (m_status) {
        case Status::DFU_ERASE_MAIN:
        case Status::DFU_ERASE_AVRDUDE:
            l.push_back("erase");
            break;
        case Status::DFU_FLASH_AVRDUDE:
        case Status::DFU_FLASH_MAIN:
            l.push_back("flash");
            l.push_back("--suppress-bootloader-mem");
            l.push_back(file);
            break;
        case Status::DFU_DISCONNECT_MAIN:
        case Status::DFU_DISCONNECT_AVRDUDE:
            l.push_back("launch");
            break;
        default:
            break;
    }
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    m_process->start(dir.filePath("dfu-programmer"), l);
}
QString Programmer::programRF(QString hexFile) {
    QFile inputFile(hexFile);
    m_tmp_hex = new QTemporaryFile(QDir::temp().filePath("XXXXXX.hex"));
    if (m_tmp_hex->open()) {
        QTextStream out(m_tmp_hex);
        if (inputFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&inputFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                // DEADBEEF is a placeholder for each rx id, and the there is a checksum of 0D by default. Replace both, strip the checksum then generate a new checksum
                // Also, flip the endianness, as QString::number is printing big endian
                if (line.contains("EFBEADDEEFBEADDE")) {
                    uint32_t tx = m_parent_device->getRFID();
                    uint32_t rx = m_parent_device->generateClientRFID();
                    QString txStr = QString::number(qToBigEndian(tx), 16);
                    QString rxStr = QString::number(qToBigEndian(rx), 16);
                    // Zero pad to the right, because endianness
                    rxStr += QString("0").repeated(8 - rxStr.length());
                    txStr += QString("0").repeated(8 - txStr.length());
                    line = line.replace("EFBEADDEEFBEADDE", txStr + rxStr).toUpper().chopped(2);
                    uint16_t checksum = 0;
                    for (int i = 1; i < line.length(); i += 2) {
                        checksum += line.mid(i, 2).toInt(NULL, 16);
                    }
                    checksum = ((~checksum) & 0xff) + 1;
                    line += QString::number(checksum, 16).toUpper();
                }
                out << line << Qt::endl;
            }
            inputFile.close();
        }
        m_tmp_hex->close();
    }
    return m_tmp_hex->fileName();
}
void Programmer::programAvrDude() {
    m_status = Status::AVRDUDE;
    statusChanged(m_status);
    board_t board = m_device->getBoard();
    QString hexFile = "ardwiino-" + board.hexFile + "-" + board.processor + "-" + QString::number(board.cpuFrequency) + (m_restore ? "-restore" : "") + (m_rf ? "-rf" : "") + ".hex";
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("firmware");
    m_process_out.clear();
    QString file = dir.filePath(hexFile);
    if (m_rf) {
        file = programRF(file);
    }
    dir.cdUp();
    dir.cd("binaries");
    dir.cd("avrdude");
    auto confDir = QDir(dir);
    confDir.cd("etc");
    dir.cd("bin");
    m_process = new QProcess();
    m_process->setWorkingDirectory(dir.path());
    QStringList l = {
        "-C",
        confDir.filePath("avrdude.conf"),
        "-c",
        board.protocol,
        "-p",
        board.processor,
        "-b",
        QString::number(board.baudRate),
        "-P",
        static_cast<SerialDevice*>(m_device)->getLocation(),
        "-U",
        "flash:w:" + file + ":a",
    };
    qDebug() << l;
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    m_process->start(dir.filePath("avrdude"), l);
}
auto Programmer::program(Device* port) -> bool {
    m_device = port;
    if (m_status == Status::WAIT) {
        if (m_device->getBoard().hasDFU) {
            DfuArduino* dfu = dynamic_cast<DfuArduino*>(m_device);
            if (dfu) {
                m_status = Status::DFU_ERASE_AVRDUDE;
                programDFU();
            } else {
                port->bootloader();
                m_status = Status::DFU_CONNECT_AVRDUDE;
            }
        } else {
            if (port->getBoard().inBootloader) {
                programAvrDude();
            } else {
                port->bootloader();
                m_status = Status::WAIT_AVRDUDE;
            }
        }
    }

    emit statusChanged(m_status);
    emit statusVChanged(getStatusDescription());
    return false;
}

void Programmer::complete(int exitCode, QProcess::ExitStatus exitStatus) {
    (void)exitStatus;
    QObject::disconnect(m_process, &QProcess::readyReadStandardOutput, this, &Programmer::onReady);
    QObject::disconnect(m_process, &QProcess::readyReadStandardError, this, &Programmer::onReady);
    QObject::disconnect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Programmer::complete);
    QObject::disconnect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    switch (m_status) {
        case Status::DFU_ERASE_AVRDUDE:
            m_status = Status::DFU_FLASH_AVRDUDE;
            programDFU();
            break;
        case Status::DFU_FLASH_AVRDUDE:
            m_status = Status::DFU_DISCONNECT_AVRDUDE;
            programDFU();
            break;
        case Status::DFU_ERASE_MAIN:
            m_status = Status::DFU_FLASH_MAIN;
            programDFU();
            break;
        case Status::DFU_FLASH_MAIN:
            m_status = Status::DFU_DISCONNECT_MAIN;
            programDFU();
            break;
        case Status::AVRDUDE:
            if (m_device->getBoard().hasDFU) {
                m_status = Status::DFU_CONNECT_MAIN;
            } else if (m_device->getBoard().shortName == "mini") {
                // Minis dont't ever need to get reconnected.
                m_status = Status::COMPLETE;
                m_rf = false;
                m_device = m_parent_device;
                m_tmp_hex = NULL;
                m_parent_device->writeConfig();
                m_parent_device = NULL;
                break;
            } else {
                m_status = Status::DISCONNECT_AVRDUDE;
            }
            if (!m_rf) {
                m_device->bootloader();
                m_device->bootloader();
            } else {
                m_tmp_hex = NULL;
            }
            break;
    }
    emit statusChanged(m_status);
    emit statusVChanged(getStatusDescription());
}

void Programmer::onReady() {
    QString out = m_process->readAllStandardOutput();
    QString out2 = m_process->readAllStandardError();
    m_process_out += out2;
    m_process_out += out;
    if (m_restore) {
        m_process_percent += out2.count('#') * ((100.0 / 50.0) / 300.0);
        m_process_percent += out2.count('>') * ((100.0 / 32.0) / 200.0);
    } else {
        bool hasDfu = m_device->getBoard().hasDFU;
        //For avrdude, each # counts for 2%, and there are 4 steps, so 400% total. 2/400 rescales that back to 100%.
        //For dfu, there are a total of 32 >'s per line, and there are two steps.
        //There are three different processes, two dfu and one avrdude. we need 100%, so we can do two 33s and one 34.
        m_process_percent += out2.count('#') * ((100.0 / 50.0) / 300.0) * (hasDfu ? 0.34 : 1);
        m_process_percent += out2.count('>') * ((100.0 / 32.0) / 200.0) * (hasDfu ? 0.33 : 0);
    }
    emit processOutChanged(m_process_out);
    emit processPercentChanged(m_process_percent);
    emit statusVChanged(getStatusDescription());
}
