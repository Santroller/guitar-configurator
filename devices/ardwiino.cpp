#include "ardwiino.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QSettings>
#include <QThread>

#include "submodules/Ardwiino/src/shared/config/defaults.h"
Ardwiino::Ardwiino(UsbDevice_t devt, QObject* parent) : Device(devt, parent), m_usbDevice(&m_deviceID), m_isOpen(false), m_configurable(false) {
}
bool Ardwiino::open() {
    if (m_deviceID.serial.isEmpty()) return false;
    if (!m_usbDevice.open()) return false;
    m_usbDevice.read(COMMAND_GET_CPU_INFO);
    m_isOpen = true;
    cpu_info_t info;
    memcpy(&info, m_usbDevice.read(COMMAND_GET_CPU_INFO).data(), sizeof(info));
    if (QString::fromUtf8(info.board).isEmpty()) {
        memcpy(&info, m_usbDevice.read(COMMAND_GET_CPU_INFO).data() + 1, sizeof(info));
    }
    m_board = ArdwiinoLookup::findByBoard(QString::fromUtf8(info.board), false);
    m_board.cpuFrequency = info.cpu_freq;
    memcpy(&info, m_usbDevice.read(COMMAND_GET_RF_CPU_INFO).data(), sizeof(info));
    QString err = QString::fromUtf8((char*)&info);
    if (QString::fromUtf8(info.board).isEmpty()) {
        memcpy(&info, m_usbDevice.read(COMMAND_GET_RF_CPU_INFO).data() + 1, sizeof(info));
    }
    m_board_rf = ArdwiinoLookup::findByBoard(QString::fromUtf8(info.board), false);
    m_board_rf.cpuFrequency = info.cpu_freq;
    m_rfID = info.rfID;
    m_configurable = !ArdwiinoLookup::isOutdatedArdwiino(m_deviceID.releaseNumber);
    if (m_configurable) {
        Configuration_t conf;
        uint offset = 0;
        uint offsetId = 0;
        while (offset < sizeof(Configuration_t)) {
            auto data = m_usbDevice.read(COMMAND_READ_CONFIG + offsetId);
            memcpy(((uint8_t*)&conf) + offset, data.data(), data.length());
            offset += data.length();
            offsetId++;
        }
        m_configuration = new DeviceConfiguration(conf);
        m_default_configuration = new DeviceConfiguration(ArdwiinoDefines::getDefaultConfig());
        m_last_configuration = new DeviceConfiguration(conf);
    }
    emit configurationChanged();
    emit configurableChanged();
    emit boardImageChanged();
    return true;
}
void Ardwiino::writeChunked(uint8_t cmd, QByteArray dataToWrite) {
    // If the guitar has been unplugged do not attempt to send it any data
    if (isReady()) {
        uint offset = 0;
        uint offsetId = 0;
        QByteArray data;
        while (offset < sizeof(Configuration_t)) {
            data.clear();
            data.push_back(offsetId);
            data.push_back(dataToWrite.mid(offset, PACKET_SIZE));
            m_usbDevice.write(cmd, data);
            offset += PACKET_SIZE;
            offsetId++;
            if (cmd == COMMAND_WRITE_CONFIG) {
                // Delay to give time to write to eeprom
                QThread::currentThread()->msleep(200);
            }
        }
    }
}
void Ardwiino::writeConfig() {
    auto config = m_configuration->getConfig();
    auto configCh = reinterpret_cast<char*>(&config);
    writeChunked(COMMAND_WRITE_CONFIG, QByteArray::fromRawData(configCh, sizeof(Configuration_t)));

    uint8_t st = config.main.subType;
    if (m_configuration->isDrum() && m_configuration->isXInput()) {
        st = REAL_DRUM_SUBTYPE;
    } else if (m_configuration->isGuitar() && m_configuration->isXInput()) {
        st = REAL_GUITAR_SUBTYPE;
    }
    m_usbDevice.write(COMMAND_WRITE_SUBTYPE, QByteArray(1, st));
    QThread::currentThread()->msleep(500);
    m_usbDevice.write(COMMAND_REBOOT, QByteArray(1, 0x00));
    // m_hiddev = NULL;
}

qint32 Ardwiino::generateClientRFID() {
    qint32 id = QRandomGenerator::global()->generate();
    m_configuration->setRfId(QRandomGenerator::global()->generate());
    return id;
}
void Ardwiino::findDigital(QJSValue callback) {
    if (isReady()) {
        m_pinDetectionCallback = callback;
        m_usbDevice.write(COMMAND_FIND_DIGITAL, QByteArray(1, 0x00));
        QTimer::singleShot(500, [&]() {
            if (isReady()) {
                uint8_t pin = m_usbDevice.read(COMMAND_GET_FOUND)[0];
                if (pin == 0xFF) {
                    if (m_hasPinDetectionCallback) {
                        findDigital(m_pinDetectionCallback);
                    }
                } else {
                    QJSValueList args;
                    args << QJSValue(pin);
                    m_pinDetectionCallback.call(args);
                }
            }
        });
    }
}
int Ardwiino::readAnalog(int pin) {
    if (isReady()) {
        int16_t axis[XBOX_AXIS_COUNT];
        memcpy(&axis, m_usbDevice.read(COMMAND_GET_VALUES).data(), sizeof(axis));
        return axis[pin];
    }
    return 0;
}
void Ardwiino::findAnalog(QJSValue callback) {
    if (isReady()) {
        m_pinDetectionCallback = callback;
        m_usbDevice.write(COMMAND_FIND_ANALOG, QByteArray(1, 0x00));
        QTimer::singleShot(500, [&]() {
            if (isReady()) {
                uint8_t pin = m_usbDevice.read(COMMAND_GET_FOUND)[0];
                if (pin == 0xFF) {
                    if (m_hasPinDetectionCallback) {
                        findAnalog(m_pinDetectionCallback);
                    }
                } else {
                    QJSValueList args;
                    args << QJSValue(pin);
                    m_pinDetectionCallback.call(args);
                }
            }
        });
    }
}
void Ardwiino::startFind() {
    m_hasPinDetectionCallback = true;
}
void Ardwiino::cancelFind() {
    m_hasPinDetectionCallback = false;
}
QString Ardwiino::getDescription() {
    if (!isReady()) {
        return "Ardwiino - Unable to communicate";
    }
    QString bname = m_board.name;
    if (m_board.cpuFrequency == 16000000) {
        bname = bname + " (5V)";
    } else if (m_board.cpuFrequency == 8000000) {
        bname = bname + " (3.3V)";
    }
    if (!m_configurable) {
        return "Ardwiino - " + bname + " - Outdated - Continue to update";
    }
    QString desc = "Ardwiino - " + bname + " - " + ArdwiinoDefines::getName(m_configuration->getMainSubType());
    auto ext = m_usbDevice.read(COMMAND_GET_EXTENSION);
    m_extension = ext[0] | (ext[1] << 8);
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
    if (m_configuration->getRfRfInEnabled()) {
        desc += " - RF (";
        desc += m_board_rf.name;
        desc += ")";
    }
    // On windows, we can actually write the description to registry in a way that applications will pick it up.
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\System\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_1209&PID_2882", QSettings::NativeFormat);
    settings.setValue("OEMName", desc);
#endif
    return desc;
}
bool Ardwiino::isReady() {
    return m_isOpen;
}
void Ardwiino::close() {
    if (m_isOpen) {
        m_usbDevice.close();
    }
    m_isOpen = false;
}
void Ardwiino::resetConfig() {
    m_usbDevice.write(COMMAND_RESET, QByteArray(1, 0x00));
    m_usbDevice.write(COMMAND_WRITE_SUBTYPE, QByteArray(1, ArdwiinoDefines::XINPUT_GUITAR_HERO_GUITAR));
    QThread::currentThread()->msleep(500);
    m_usbDevice.write(COMMAND_REBOOT, QByteArray(1, 0x00));
}
void Ardwiino::bootloader() {
    m_usbDevice.write(COMMAND_JUMP_BOOTLOADER, QByteArray(1, 0x00));
}

void Ardwiino::savePreset(QString name, QString config) {
    QMap<QString, QVariant> configs;
    if (settings.contains("presets")) {
        configs = settings.value("presets").toMap();
    }
    configs[name] = config;
    settings.setValue("presets", configs);
    presetsChanged();
}

QStringList Ardwiino::getPresets() {
    if (settings.contains("presets")) {
        return settings.value("presets").toMap().keys();
    }
    return QStringList();
}

QString Ardwiino::getPreset(QString name) {
    return settings.value("presets").toMap()[name].toString();
}

void Ardwiino::removePreset(QString name) {
    if (settings.contains("presets")) {
        auto map = settings.value("presets").toMap();
        map.remove(name);
        settings.setValue("presets", map);
    }
    presetsChanged();
}
void Ardwiino::importPreset(QString fileName) {
    QMap<QString, QVariant> configs;
    if (settings.contains("presets")) {
        configs = settings.value("presets").toMap();
    }
    QFile file(QDir::toNativeSeparators(QUrl(fileName).toLocalFile()));
    file.open(QIODevice::ReadOnly);

    QJsonObject doc = QJsonDocument::fromJson(file.readAll()).object();
    configs[doc["name"].toString()] = doc["config"].toString();
    settings.setValue("presets", configs);
    presetsChanged();
}
void Ardwiino::exportPreset(QString name, QString fileName) {
    QJsonObject save;
    save["name"] = name;
    save["config"] = settings.value("presets").toMap()[name].toString();
    QFile file(QDir::toNativeSeparators(QUrl(fileName).toLocalFile()));
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(save).toJson());
}