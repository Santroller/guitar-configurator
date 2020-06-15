#ifndef ARDWIINOLOOKUP_H
#define ARDWIINOLOOKUP_H
#include <QKeySequence>
#include <QList>
#include <QObject>
#include <QSerialPortInfo>
#include <QTimer>
#include <QVersionNumber>

#include "ardwiino_defines.h"
#include "submodules/Ardwiino/src/shared/config/config.h"
#define ARDWIINO_VID 0x1209
#define ARDWIINO_PID 0x2882
#define SONY_VID 0x12ba
#define HARMONIX_VID 0x1bad
#define SWITCH_VID 0x0f0d

typedef struct {
    QString shortName;
    QString hexFile;
    QString name;
    uint baudRate;
    int productIDs[9];
    QString protocol;
    QString processor;
    uint cpuFrequency;
    QString image;
    bool hasDFU;
} board_t;

class ArdwiinoLookup : public QObject {
    Q_OBJECT
   public:
    static ArdwiinoLookup* getInstance();
    static const board_t boards[11];
    static const board_t detectBoard(struct hid_device_info *usbDeviceId);
    static const board_t detectBoard(const QSerialPortInfo& serialPortInfo);
    static const board_t empty;
    explicit ArdwiinoLookup(QObject* parent = nullptr);
    static const board_t findByBoard(const QString& board);
   public slots:
    static bool isArdwiino(const QSerialPortInfo& info);
    static bool isArdwiino(struct hid_device_info *usbDeviceId);
    static bool isOldAPIArdwiino(const QSerialPortInfo& QSerialPort);
    static bool isAreadyDFU(const QSerialPortInfo& QSerialPort);

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
