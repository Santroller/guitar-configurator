#ifndef ARDWIINOLOOKUP_H
#define ARDWIINOLOOKUP_H
#include <QObject>
#include <QTimer>
#include <QSerialPortInfo>
#include <QKeySequence>
#include <QList>
#include "submodules/Ardwiino/src/shared/input/input_wii_ext.h"
#include "submodules/Ardwiino/src/shared/config/config.h"
#include "submodules/Ardwiino/src/shared/config/defaults.h"
#include "submodules/Ardwiino/src/shared/controller/controller.h"
#include "controllers.h"
#include "input_types.h"
#include "joy_types.h"
#include "mpu_orientations.h"
#include "tilt_types.h"
#define ARDWIINO_VID 0x1209
#define ARDWIINO_PID 0x2882
#define SONY_VID 0x12ba
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
    QString originalFirmware;
    QString image;
    bool hasDFU;
} board_t;

class ArdwiinoLookup: public QObject
{
    Q_OBJECT
public:
    static ArdwiinoLookup* getInstance();
    static const board_t boards[5];
    static const board_t detectBoard(const QSerialPortInfo &serialPortInfo);
    static const board_t empty;
    explicit ArdwiinoLookup(QObject *parent = nullptr);
    static const board_t findByBoard(const QString& board);
public slots:
    static QString lookupExtension(uint8_t type, uint16_t device);
    static QString lookupType(uint8_t type);
    static bool isArdwiino(const QSerialPortInfo &info);
    static bool isOldFirmwareArdwiino(const QSerialPortInfo& QSerialPort);
    static bool isOldArdwiino(const QSerialPortInfo& QSerialPort);
    inline QString getControllerTypeName(Controllers::Value value) {
        return Controllers::toString(value);
    }
    inline QString getInputTypeName(InputTypes::Value value) {
        return InputTypes::toString(value);
    }
    inline QString getJoyTypeName(JoyTypes::Value value) {
        return JoyTypes::toString(value);
    }
    inline QString getOrientationName(MPU6050Orientations::Value value) {
        return MPU6050Orientations::toString(value);
    }
    inline QString getTiltTypeName(TiltTypes::Value value) {
        return TiltTypes::toString(value);
    }
    inline QString getKeyName(QVariantList sequence) {
        if (sequence.length() == 2) {
            return QKeySequence(sequence[0].toInt(), sequence[1].toInt()).toString();
        }
        return QKeySequence(sequence[0].toInt()).toString();
    }
private:
    static ArdwiinoLookup* instance;
    static float currentVersion;
};

#endif // ARDWIINOLOOKUP_H
