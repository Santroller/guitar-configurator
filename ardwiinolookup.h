#ifndef ARDWIINOLOOKUP_H
#define ARDWIINOLOOKUP_H
#include <QKeySequence>
#include <QList>
#include <QObject>
#include <QSerialPortInfo>
#include <QTimer>
#include <QVersionNumber>

#include "ardwiino_defines.h"
#include "devices/device.h"
#include "submodules/Ardwiino/src/shared/config/config.h"
#define ARDWIINO_VID 0x1209
#define ARDWIINO_PID 0x2882
#define SONY_VID 0x12ba
#define HARMONIX_VID 0x1bad
#define SWITCH_VID 0x0f0d
#define RASPBERRY_PI_VID 0x2e8a
#define PICOBOOT_PID 0x0003

class ArdwiinoLookup : public QObject {
    Q_OBJECT
   public:
    static ArdwiinoLookup* getInstance();
    static const board_t boards[37];
    static const board_t detectBoard(const QSerialPortInfo& serialPortInfo);
    static const board_t empty;
    static const board_t pico;
    explicit ArdwiinoLookup(QObject* parent = nullptr);
    static const board_t findByBoard(const QString& board, bool inBootloader);
   public slots:
    static bool isArdwiino(const QSerialPortInfo& info);
    static bool isArdwiino(const UsbDevice_t& desc);
    static bool isOldAPIArdwiino(const QSerialPortInfo& QSerialPort);
    static bool isOutdatedArdwiino(const unsigned short releaseID);

    inline QString getTypeName(uint8_t type) {
        return ArdwiinoDefines::getName(ArdwiinoDefines::SubType(type));
    }

    inline QString getInputTypeName(ArdwiinoDefines::InputType value) {
        return ArdwiinoDefines::getName(value);
    }
    inline QString getOrientationName(ArdwiinoDefines::GyroOrientation value) {
        return ArdwiinoDefines::getName(value);
    }
    inline QString getTiltTypeName(ArdwiinoDefines::TiltType value) {
        return ArdwiinoDefines::getName(value);
    }
    inline QString getKeyName(QVariantList sequence) {
        if (sequence.length() == 2) {
            return QKeySequence(sequence[0].toInt(), sequence[1].toInt()).toString();
        }
        return QKeySequence(sequence[0].toInt()).toString();
    }

   private:
    static ArdwiinoLookup* instance;
    static QVersionNumber currentVersion;
    static QVersionNumber supports1Mhz;
    static QVersionNumber supportsCurrent;
    static QVersionNumber supportsAutoBind;
    static QVersionNumber supportsMIDI;
};

#endif  // ARDWIINOLOOKUP_H
