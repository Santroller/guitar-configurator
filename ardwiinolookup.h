#ifndef ARDWIINOLOOKUP_H
#define ARDWIINOLOOKUP_H
#include <QObject>
#include <QTimer>
#include <QSerialPortInfo>
#include <QList>
#include "submodules/Ardwiino/src/shared/input/input_wii_ext.h"
#include "submodules/Ardwiino/src/shared/config/config.h"
#include "submodules/Ardwiino/src/shared/config/defaults.h"
#include "submodules/Ardwiino/src/shared/controller/controller.h"
#include "controllers.h"
#include "input_types.h"
#include "mpu_orientations.h"
#include "tilt_types.h"
#define ARDWIINO_VID 0x1209
#define ARDWIINO_PID 0x2882
#define SONY_VID 0x12ba

typedef struct {
    QString hexFile;
    QString name;
    uint baudRate;
    int productIDs[9];
    QString protocol;
    QString processor;
    uint cpuFrequency;
    QString originalFirmware;
} board_t;

class ArdwiinoLookup: public QObject
{
    Q_OBJECT
public:
    static ArdwiinoLookup* getInstance();
    static const board_t boards[4];
    static const board_t* detectBoard(const QSerialPortInfo &serialPortInfo);
    static const board_t retriveDFUVariant(const board_t board);
    static const board_t empty;
    explicit ArdwiinoLookup(QObject *parent = nullptr);
    static const board_t findByBoard(QString board);
public slots:
    QString lookupExtension(uint8_t type, uint16_t device);
    QString lookupType(uint8_t type);
    bool isArdwiino(const QSerialPortInfo &info);
    bool hasDFUVariant(const board_t board);
    inline QString getControllerTypeName(Controllers::Value value) {
        return Controllers::toString(value);
    }
    inline QString getInputTypeName(InputTypes::Value value) {
        return InputTypes::toString(value);
    }
    inline QString getOrientationName(MPU6050Orientations::Value value) {
        return MPU6050Orientations::toString(value);
    }
    inline QString getTiltTypeName(TiltTypes::Value value) {
        return TiltTypes::toString(value);
    }
private:
    static ArdwiinoLookup* instance;
};

#endif // ARDWIINOLOOKUP_H
