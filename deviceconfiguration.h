
#include <QDebug>
#include <QMetaEnum>
#include <QObject>
#include "ardwiino_defines.h"

#include "submodules/Ardwiino/src/shared/config/config.h"
class DeviceConfiguration : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isGuitar READ isGuitar NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isDrum READ isDrum NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isLiveGuitar READ isLiveGuitar NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isKeyboard READ isKeyboard NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isRB READ isRB NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isWii READ isWii NOTIFY mainInputTypeUpdated)
    Q_PROPERTY(bool isMIDI READ isMIDI NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool isMouse READ isMouse NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(bool hasAddressableLEDs READ hasAddressableLEDs NOTIFY mainFretLEDModeUpdated)
    Q_PROPERTY(QVariantList leds READ getLEDs NOTIFY ledsUpdated)
    Q_PROPERTY(QVariantMap ledColours READ getLEDColours NOTIFY ledsUpdated)
    Q_PROPERTY(ArdwiinoDefines::InputType mainInputType READ getMainInputType WRITE setMainInputType NOTIFY mainInputTypeUpdated)
    Q_PROPERTY(ArdwiinoDefines::SubType mainSubType READ getMainSubType WRITE setMainSubType NOTIFY mainSubTypeUpdated)
    Q_PROPERTY(ArdwiinoDefines::TiltType mainTiltType READ getMainTiltType WRITE setMainTiltType NOTIFY mainTiltTypeUpdated)
    Q_PROPERTY(int mainUnused READ getMainUnused WRITE setMainUnused NOTIFY mainUnusedUpdated)
    Q_PROPERTY(ArdwiinoDefines::FretLedMode mainFretLEDMode READ getMainFretLEDMode WRITE setMainFretLEDMode NOTIFY mainFretLEDModeUpdated)
    Q_PROPERTY(bool mainMapLeftJoystickToDPad READ getMainMapLeftJoystickToDPad WRITE setMainMapLeftJoystickToDPad NOTIFY mainMapLeftJoystickToDPadUpdated)
    Q_PROPERTY(bool mainMapStartSelectToHome READ getMainMapStartSelectToHome WRITE setMainMapStartSelectToHome NOTIFY mainMapStartSelectToHomeUpdated)
    Q_PROPERTY(bool mainMapNunchukAccelToRightJoy READ getMainMapNunchukAccelToRightJoy WRITE setMainMapNunchukAccelToRightJoy NOTIFY mainMapNunchukAccelToRightJoyUpdated)
    Q_PROPERTY(ArdwiinoDefines::FirmwareType mainSignature READ getMainSignature WRITE setMainSignature NOTIFY mainSignatureUpdated)
    Q_PROPERTY(int mainVersion READ getMainVersion WRITE setMainVersion NOTIFY mainVersionUpdated)
    Q_PROPERTY(int pinsUp READ getPinsUp WRITE setPinsUp NOTIFY pinsUpUpdated)
    Q_PROPERTY(int pinsDown READ getPinsDown WRITE setPinsDown NOTIFY pinsDownUpdated)
    Q_PROPERTY(int pinsLeft READ getPinsLeft WRITE setPinsLeft NOTIFY pinsLeftUpdated)
    Q_PROPERTY(int pinsRight READ getPinsRight WRITE setPinsRight NOTIFY pinsRightUpdated)
    Q_PROPERTY(int pinsStart READ getPinsStart WRITE setPinsStart NOTIFY pinsStartUpdated)
    Q_PROPERTY(int pinsBack READ getPinsBack WRITE setPinsBack NOTIFY pinsBackUpdated)
    Q_PROPERTY(int pinsLeftStick READ getPinsLeftStick WRITE setPinsLeftStick NOTIFY pinsLeftStickUpdated)
    Q_PROPERTY(int pinsRightStick READ getPinsRightStick WRITE setPinsRightStick NOTIFY pinsRightStickUpdated)
    Q_PROPERTY(int pinsLB READ getPinsLB WRITE setPinsLB NOTIFY pinsLBUpdated)
    Q_PROPERTY(int pinsRB READ getPinsRB WRITE setPinsRB NOTIFY pinsRBUpdated)
    Q_PROPERTY(int pinsHome READ getPinsHome WRITE setPinsHome NOTIFY pinsHomeUpdated)
    Q_PROPERTY(int pinsCapture READ getPinsCapture WRITE setPinsCapture NOTIFY pinsCaptureUpdated)
    Q_PROPERTY(int pinsA READ getPinsA WRITE setPinsA NOTIFY pinsAUpdated)
    Q_PROPERTY(int pinsB READ getPinsB WRITE setPinsB NOTIFY pinsBUpdated)
    Q_PROPERTY(int pinsX READ getPinsX WRITE setPinsX NOTIFY pinsXUpdated)
    Q_PROPERTY(int pinsY READ getPinsY WRITE setPinsY NOTIFY pinsYUpdated)
    Q_PROPERTY(int pinsLt READ getPinsLt WRITE setPinsLt NOTIFY pinsLtUpdated)
    Q_PROPERTY(bool pinsLtInverted READ getPinsLtInverted WRITE setPinsLtInverted NOTIFY pinsLtInvertedUpdated)
    Q_PROPERTY(int pinsRt READ getPinsRt WRITE setPinsRt NOTIFY pinsRtUpdated)
    Q_PROPERTY(bool pinsRtInverted READ getPinsRtInverted WRITE setPinsRtInverted NOTIFY pinsRtInvertedUpdated)
    Q_PROPERTY(int pinsLX READ getPinsLX WRITE setPinsLX NOTIFY pinsLXUpdated)
    Q_PROPERTY(bool pinsLXInverted READ getPinsLXInverted WRITE setPinsLXInverted NOTIFY pinsLXInvertedUpdated)
    Q_PROPERTY(int pinsLY READ getPinsLY WRITE setPinsLY NOTIFY pinsLYUpdated)
    Q_PROPERTY(bool pinsLYInverted READ getPinsLYInverted WRITE setPinsLYInverted NOTIFY pinsLYInvertedUpdated)
    Q_PROPERTY(int pinsRX READ getPinsRX WRITE setPinsRX NOTIFY pinsRXUpdated)
    Q_PROPERTY(bool pinsRXInverted READ getPinsRXInverted WRITE setPinsRXInverted NOTIFY pinsRXInvertedUpdated)
    Q_PROPERTY(int pinsRY READ getPinsRY WRITE setPinsRY NOTIFY pinsRYUpdated)
    Q_PROPERTY(bool pinsRYInverted READ getPinsRYInverted WRITE setPinsRYInverted NOTIFY pinsRYInvertedUpdated)
    Q_PROPERTY(int axisTriggerThreshold READ getAxisTriggerThreshold WRITE setAxisTriggerThreshold NOTIFY axisTriggerThresholdUpdated)
    Q_PROPERTY(int axisJoyThreshold READ getAxisJoyThreshold WRITE setAxisJoyThreshold NOTIFY axisJoyThresholdUpdated)
    Q_PROPERTY(ArdwiinoDefines::GyroOrientation axisMpu6050Orientation READ getAxisMpu6050Orientation WRITE setAxisMpu6050Orientation NOTIFY axisMpu6050OrientationUpdated)
    Q_PROPERTY(int axisTiltSensitivity READ getAxisTiltSensitivity WRITE setAxisTiltSensitivity NOTIFY axisTiltSensitivityUpdated)
    Q_PROPERTY(int keysUp READ getKeysUp WRITE setKeysUp NOTIFY keysUpUpdated)
    Q_PROPERTY(int keysDown READ getKeysDown WRITE setKeysDown NOTIFY keysDownUpdated)
    Q_PROPERTY(int keysLeft READ getKeysLeft WRITE setKeysLeft NOTIFY keysLeftUpdated)
    Q_PROPERTY(int keysRight READ getKeysRight WRITE setKeysRight NOTIFY keysRightUpdated)
    Q_PROPERTY(int keysStart READ getKeysStart WRITE setKeysStart NOTIFY keysStartUpdated)
    Q_PROPERTY(int keysBack READ getKeysBack WRITE setKeysBack NOTIFY keysBackUpdated)
    Q_PROPERTY(int keysLeftStick READ getKeysLeftStick WRITE setKeysLeftStick NOTIFY keysLeftStickUpdated)
    Q_PROPERTY(int keysRightStick READ getKeysRightStick WRITE setKeysRightStick NOTIFY keysRightStickUpdated)
    Q_PROPERTY(int keysLB READ getKeysLB WRITE setKeysLB NOTIFY keysLBUpdated)
    Q_PROPERTY(int keysRB READ getKeysRB WRITE setKeysRB NOTIFY keysRBUpdated)
    Q_PROPERTY(int keysHome READ getKeysHome WRITE setKeysHome NOTIFY keysHomeUpdated)
    Q_PROPERTY(int keysCapture READ getKeysCapture WRITE setKeysCapture NOTIFY keysCaptureUpdated)
    Q_PROPERTY(int keysA READ getKeysA WRITE setKeysA NOTIFY keysAUpdated)
    Q_PROPERTY(int keysB READ getKeysB WRITE setKeysB NOTIFY keysBUpdated)
    Q_PROPERTY(int keysX READ getKeysX WRITE setKeysX NOTIFY keysXUpdated)
    Q_PROPERTY(int keysY READ getKeysY WRITE setKeysY NOTIFY keysYUpdated)
    Q_PROPERTY(int keysLt READ getKeysLt WRITE setKeysLt NOTIFY keysLtUpdated)
    Q_PROPERTY(int keysRt READ getKeysRt WRITE setKeysRt NOTIFY keysRtUpdated)
    Q_PROPERTY(int keysLXNeg READ getKeysLXNeg WRITE setKeysLXNeg NOTIFY keysLXNegUpdated)
    Q_PROPERTY(int keysLXPos READ getKeysLXPos WRITE setKeysLXPos NOTIFY keysLXPosUpdated)
    Q_PROPERTY(int keysLYNeg READ getKeysLYNeg WRITE setKeysLYNeg NOTIFY keysLYNegUpdated)
    Q_PROPERTY(int keysLYPos READ getKeysLYPos WRITE setKeysLYPos NOTIFY keysLYPosUpdated)
    Q_PROPERTY(int keysRXNeg READ getKeysRXNeg WRITE setKeysRXNeg NOTIFY keysRXNegUpdated)
    Q_PROPERTY(int keysRXPos READ getKeysRXPos WRITE setKeysRXPos NOTIFY keysRXPosUpdated)
    Q_PROPERTY(int keysRYNeg READ getKeysRYNeg WRITE setKeysRYNeg NOTIFY keysRYNegUpdated)
    Q_PROPERTY(int keysRYPos READ getKeysRYPos WRITE setKeysRYPos NOTIFY keysRYPosUpdated)
    Q_PROPERTY(int drumThreshold READ getDrumThreshold WRITE setDrumThreshold NOTIFY drumThresholdUpdated)
    Q_PROPERTY(QVariantMap midiType READ getMidiType NOTIFY midiTypeUpdated)
    Q_PROPERTY(QVariantMap midiNote READ getMidiNote NOTIFY midiNoteUpdated)
    Q_PROPERTY(QVariantMap midiChannel READ getMidiChannel NOTIFY midiChannelUpdated)

public:
    explicit DeviceConfiguration(Configuration_t config, QObject* parent = nullptr);
public slots:
    bool isGuitar() {
        return ArdwiinoDefines::getName(getMainSubType()).toLower().contains("guitar");
    }
    bool isDrum() {
        return ArdwiinoDefines::getName(getMainSubType()).toLower().contains("drum");
    }
    bool isMIDI() {
        return ArdwiinoDefines::getName(getMainSubType()).toLower().contains("midi");
    }
    bool isKeyboard() {
        return getMainSubType() >= ArdwiinoDefines::KEYBOARD_GAMEPAD && getMainSubType() <= ArdwiinoDefines::KEYBOARD_ROCK_BAND_DRUMS;
    }
    bool isMouse() {
        return getMainSubType() == ArdwiinoDefines::MOUSE;
    }
    bool isLiveGuitar() {
        return getMainSubType() == ArdwiinoDefines::XINPUT_LIVE_GUITAR;
    }
    bool isWii() {
        return getMainInputType() == ArdwiinoDefines::WII;
    }
    bool isRB() {
        return ArdwiinoDefines::getName(getMainSubType()).toLower().contains("rock");
    }
    bool hasAddressableLEDs() {
        return getMainFretLEDMode() == ArdwiinoDefines::APA102;
    }
    Configuration_t getConfig() const {
        return m_config;
    }

    ArdwiinoDefines::InputType getMainInputType() const {
        return (ArdwiinoDefines::InputType)m_config.main.inputType;
    }
    void setMainInputType(ArdwiinoDefines::InputType val) {
        m_config.main.inputType = (ArdwiinoDefines::InputType)val;
        emit mainInputTypeUpdated();
    }
    ArdwiinoDefines::SubType getMainSubType() const {
        return (ArdwiinoDefines::SubType)m_config.main.subType;
    }
    void setMainSubType(ArdwiinoDefines::SubType val) {
        m_config.main.subType = (ArdwiinoDefines::SubType)val;
        emit mainSubTypeUpdated();
    }
    ArdwiinoDefines::TiltType getMainTiltType() const {
        return (ArdwiinoDefines::TiltType)m_config.main.tiltType;
    }
    void setMainTiltType(ArdwiinoDefines::TiltType val) {
        m_config.main.tiltType = (ArdwiinoDefines::TiltType)val;
        emit mainTiltTypeUpdated();
    }
    int getMainUnused() const {
        return (uint8_t)m_config.main.unused;
    }
    void setMainUnused(int val) {
        m_config.main.unused = (uint8_t)val;
        emit mainUnusedUpdated();
    }
    ArdwiinoDefines::FretLedMode getMainFretLEDMode() const {
        return (ArdwiinoDefines::FretLedMode)m_config.main.fretLEDMode;
    }
    void setMainFretLEDMode(ArdwiinoDefines::FretLedMode val) {
        m_config.main.fretLEDMode = (ArdwiinoDefines::FretLedMode)val;
        emit mainFretLEDModeUpdated();
    }
    bool getMainMapLeftJoystickToDPad() const {
        return m_config.main.mapLeftJoystickToDPad;
    }
    void setMainMapLeftJoystickToDPad(bool val) {
        m_config.main.mapLeftJoystickToDPad = val;
        emit mainMapLeftJoystickToDPadUpdated();
    }
    bool getMainMapStartSelectToHome() const {
        return m_config.main.mapStartSelectToHome;
    }
    void setMainMapStartSelectToHome(bool val) {
        m_config.main.mapStartSelectToHome = val;
        emit mainMapStartSelectToHomeUpdated();
    }
    bool getMainMapNunchukAccelToRightJoy() const {
        return m_config.main.mapNunchukAccelToRightJoy;
    }
    void setMainMapNunchukAccelToRightJoy(bool val) {
        m_config.main.mapNunchukAccelToRightJoy = val;
        emit mainMapNunchukAccelToRightJoyUpdated();
    }
    ArdwiinoDefines::FirmwareType getMainSignature() const {
        return (ArdwiinoDefines::FirmwareType)m_config.main.signature;
    }
    void setMainSignature(ArdwiinoDefines::FirmwareType val) {
        m_config.main.signature = (ArdwiinoDefines::FirmwareType)val;
        emit mainSignatureUpdated();
    }
    int getMainVersion() const {
        return (uint32_t)m_config.main.version;
    }
    void setMainVersion(int val) {
        m_config.main.version = (uint32_t)val;
        emit mainVersionUpdated();
    }
    int getPinsUp() const {
        return (uint8_t)m_config.pins.up;
    }
    void setPinsUp(int val) {
        m_config.pins.up = (uint8_t)val;
        emit pinsUpUpdated();
    }
    int getPinsDown() const {
        return (uint8_t)m_config.pins.down;
    }
    void setPinsDown(int val) {
        m_config.pins.down = (uint8_t)val;
        emit pinsDownUpdated();
    }
    int getPinsLeft() const {
        return (uint8_t)m_config.pins.left;
    }
    void setPinsLeft(int val) {
        m_config.pins.left = (uint8_t)val;
        emit pinsLeftUpdated();
    }
    int getPinsRight() const {
        return (uint8_t)m_config.pins.right;
    }
    void setPinsRight(int val) {
        m_config.pins.right = (uint8_t)val;
        emit pinsRightUpdated();
    }
    int getPinsStart() const {
        return (uint8_t)m_config.pins.start;
    }
    void setPinsStart(int val) {
        m_config.pins.start = (uint8_t)val;
        emit pinsStartUpdated();
    }
    int getPinsBack() const {
        return (uint8_t)m_config.pins.back;
    }
    void setPinsBack(int val) {
        m_config.pins.back = (uint8_t)val;
        emit pinsBackUpdated();
    }
    int getPinsLeftStick() const {
        return (uint8_t)m_config.pins.left_stick;
    }
    void setPinsLeftStick(int val) {
        m_config.pins.left_stick = (uint8_t)val;
        emit pinsLeftStickUpdated();
    }
    int getPinsRightStick() const {
        return (uint8_t)m_config.pins.right_stick;
    }
    void setPinsRightStick(int val) {
        m_config.pins.right_stick = (uint8_t)val;
        emit pinsRightStickUpdated();
    }
    int getPinsLB() const {
        return (uint8_t)m_config.pins.LB;
    }
    void setPinsLB(int val) {
        m_config.pins.LB = (uint8_t)val;
        emit pinsLBUpdated();
    }
    int getPinsRB() const {
        return (uint8_t)m_config.pins.RB;
    }
    void setPinsRB(int val) {
        m_config.pins.RB = (uint8_t)val;
        emit pinsRBUpdated();
    }
    int getPinsHome() const {
        return (uint8_t)m_config.pins.home;
    }
    void setPinsHome(int val) {
        m_config.pins.home = (uint8_t)val;
        emit pinsHomeUpdated();
    }
    int getPinsCapture() const {
        return (uint8_t)m_config.pins.capture;
    }
    void setPinsCapture(int val) {
        m_config.pins.capture = (uint8_t)val;
        emit pinsCaptureUpdated();
    }
    int getPinsA() const {
        return (uint8_t)m_config.pins.a;
    }
    void setPinsA(int val) {
        m_config.pins.a = (uint8_t)val;
        emit pinsAUpdated();
    }
    int getPinsB() const {
        return (uint8_t)m_config.pins.b;
    }
    void setPinsB(int val) {
        m_config.pins.b = (uint8_t)val;
        emit pinsBUpdated();
    }
    int getPinsX() const {
        return (uint8_t)m_config.pins.x;
    }
    void setPinsX(int val) {
        m_config.pins.x = (uint8_t)val;
        emit pinsXUpdated();
    }
    int getPinsY() const {
        return (uint8_t)m_config.pins.y;
    }
    void setPinsY(int val) {
        m_config.pins.y = (uint8_t)val;
        emit pinsYUpdated();
    }
    int getPinsLt() const {
        return (uint8_t)m_config.pins.lt.pin;
    }
    void setPinsLt(int val) {
        m_config.pins.lt.pin = (uint8_t)val;
        emit pinsLtUpdated();
    }
    bool getPinsLtInverted() const {
        return m_config.pins.lt.inverted;
    }
    void setPinsLtInverted(bool val) {
        m_config.pins.lt.inverted = val;
        emit pinsLtInvertedUpdated();
    }
    int getPinsRt() const {
        return (uint8_t)m_config.pins.rt.pin;
    }
    void setPinsRt(int val) {
        m_config.pins.rt.pin = (uint8_t)val;
        emit pinsRtUpdated();
    }
    bool getPinsRtInverted() const {
        return m_config.pins.rt.inverted;
    }
    void setPinsRtInverted(bool val) {
        m_config.pins.rt.inverted = val;
        emit pinsRtInvertedUpdated();
    }
    int getPinsLX() const {
        return (uint8_t)m_config.pins.l_x.pin;
    }
    void setPinsLX(int val) {
        m_config.pins.l_x.pin = (uint8_t)val;
        emit pinsLXUpdated();
    }
    bool getPinsLXInverted() const {
        return m_config.pins.l_x.inverted;
    }
    void setPinsLXInverted(bool val) {
        m_config.pins.l_x.inverted = val;
        emit pinsLXInvertedUpdated();
    }
    int getPinsLY() const {
        return (uint8_t)m_config.pins.l_y.pin;
    }
    void setPinsLY(int val) {
        m_config.pins.l_y.pin = (uint8_t)val;
        emit pinsLYUpdated();
    }
    bool getPinsLYInverted() const {
        return m_config.pins.l_y.inverted;
    }
    void setPinsLYInverted(bool val) {
        m_config.pins.l_y.inverted = val;
        emit pinsLYInvertedUpdated();
    }
    int getPinsRX() const {
        return (uint8_t)m_config.pins.r_x.pin;
    }
    void setPinsRX(int val) {
        m_config.pins.r_x.pin = (uint8_t)val;
        emit pinsRXUpdated();
    }
    bool getPinsRXInverted() const {
        return m_config.pins.r_x.inverted;
    }
    void setPinsRXInverted(bool val) {
        m_config.pins.r_x.inverted = val;
        emit pinsRXInvertedUpdated();
    }
    int getPinsRY() const {
        return (uint8_t)m_config.pins.r_y.pin;
    }
    void setPinsRY(int val) {
        m_config.pins.r_y.pin = (uint8_t)val;
        emit pinsRYUpdated();
    }
    bool getPinsRYInverted() const {
        return m_config.pins.r_y.inverted;
    }
    void setPinsRYInverted(bool val) {
        m_config.pins.r_y.inverted = val;
        emit pinsRYInvertedUpdated();
    }
    int getAxisTriggerThreshold() const {
        return (uint8_t)m_config.axis.triggerThreshold;
    }
    void setAxisTriggerThreshold(int val) {
        m_config.axis.triggerThreshold = (uint8_t)val;
        emit axisTriggerThresholdUpdated();
    }
    int getAxisJoyThreshold() const {
        return (uint8_t)m_config.axis.joyThreshold;
    }
    void setAxisJoyThreshold(int val) {
        m_config.axis.joyThreshold = (uint8_t)val;
        emit axisJoyThresholdUpdated();
    }
    ArdwiinoDefines::GyroOrientation getAxisMpu6050Orientation() const {
        return (ArdwiinoDefines::GyroOrientation)m_config.axis.mpu6050Orientation;
    }
    void setAxisMpu6050Orientation(ArdwiinoDefines::GyroOrientation val) {
        m_config.axis.mpu6050Orientation = (ArdwiinoDefines::GyroOrientation)val;
        emit axisMpu6050OrientationUpdated();
    }
    int getAxisTiltSensitivity() const {
        return (int16_t)m_config.axis.tiltSensitivity;
    }
    void setAxisTiltSensitivity(int val) {
        m_config.axis.tiltSensitivity = (int16_t)val;
        emit axisTiltSensitivityUpdated();
    }
    int getKeysUp() const {
        return (uint8_t)m_config.keys.up;
    }
    void setKeysUp(int val) {
        m_config.keys.up = (uint8_t)val;
        emit keysUpUpdated();
    }
    int getKeysDown() const {
        return (uint8_t)m_config.keys.down;
    }
    void setKeysDown(int val) {
        m_config.keys.down = (uint8_t)val;
        emit keysDownUpdated();
    }
    int getKeysLeft() const {
        return (uint8_t)m_config.keys.left;
    }
    void setKeysLeft(int val) {
        m_config.keys.left = (uint8_t)val;
        emit keysLeftUpdated();
    }
    int getKeysRight() const {
        return (uint8_t)m_config.keys.right;
    }
    void setKeysRight(int val) {
        m_config.keys.right = (uint8_t)val;
        emit keysRightUpdated();
    }
    int getKeysStart() const {
        return (uint8_t)m_config.keys.start;
    }
    void setKeysStart(int val) {
        m_config.keys.start = (uint8_t)val;
        emit keysStartUpdated();
    }
    int getKeysBack() const {
        return (uint8_t)m_config.keys.back;
    }
    void setKeysBack(int val) {
        m_config.keys.back = (uint8_t)val;
        emit keysBackUpdated();
    }
    int getKeysLeftStick() const {
        return (uint8_t)m_config.keys.left_stick;
    }
    void setKeysLeftStick(int val) {
        m_config.keys.left_stick = (uint8_t)val;
        emit keysLeftStickUpdated();
    }
    int getKeysRightStick() const {
        return (uint8_t)m_config.keys.right_stick;
    }
    void setKeysRightStick(int val) {
        m_config.keys.right_stick = (uint8_t)val;
        emit keysRightStickUpdated();
    }
    int getKeysLB() const {
        return (uint8_t)m_config.keys.LB;
    }
    void setKeysLB(int val) {
        m_config.keys.LB = (uint8_t)val;
        emit keysLBUpdated();
    }
    int getKeysRB() const {
        return (uint8_t)m_config.keys.RB;
    }
    void setKeysRB(int val) {
        m_config.keys.RB = (uint8_t)val;
        emit keysRBUpdated();
    }
    int getKeysHome() const {
        return (uint8_t)m_config.keys.home;
    }
    void setKeysHome(int val) {
        m_config.keys.home = (uint8_t)val;
        emit keysHomeUpdated();
    }
    int getKeysCapture() const {
        return (uint8_t)m_config.keys.capture;
    }
    void setKeysCapture(int val) {
        m_config.keys.capture = (uint8_t)val;
        emit keysCaptureUpdated();
    }
    int getKeysA() const {
        return (uint8_t)m_config.keys.a;
    }
    void setKeysA(int val) {
        m_config.keys.a = (uint8_t)val;
        emit keysAUpdated();
    }
    int getKeysB() const {
        return (uint8_t)m_config.keys.b;
    }
    void setKeysB(int val) {
        m_config.keys.b = (uint8_t)val;
        emit keysBUpdated();
    }
    int getKeysX() const {
        return (uint8_t)m_config.keys.x;
    }
    void setKeysX(int val) {
        m_config.keys.x = (uint8_t)val;
        emit keysXUpdated();
    }
    int getKeysY() const {
        return (uint8_t)m_config.keys.y;
    }
    void setKeysY(int val) {
        m_config.keys.y = (uint8_t)val;
        emit keysYUpdated();
    }
    int getKeysLt() const {
        return (uint8_t)m_config.keys.lt;
    }
    void setKeysLt(int val) {
        m_config.keys.lt = (uint8_t)val;
        emit keysLtUpdated();
    }
    int getKeysRt() const {
        return (uint8_t)m_config.keys.rt;
    }
    void setKeysRt(int val) {
        m_config.keys.rt = (uint8_t)val;
        emit keysRtUpdated();
    }
    int getKeysLXNeg() const {
        return (uint8_t)m_config.keys.l_x.neg;
    }
    void setKeysLXNeg(int val) {
        m_config.keys.l_x.neg = (uint8_t)val;
        emit keysLXNegUpdated();
    }
    int getKeysLXPos() const {
        return (uint8_t)m_config.keys.l_x.pos;
    }
    void setKeysLXPos(int val) {
        m_config.keys.l_x.pos = (uint8_t)val;
        emit keysLXPosUpdated();
    }
    int getKeysLYNeg() const {
        return (uint8_t)m_config.keys.l_y.neg;
    }
    void setKeysLYNeg(int val) {
        m_config.keys.l_y.neg = (uint8_t)val;
        emit keysLYNegUpdated();
    }
    int getKeysLYPos() const {
        return (uint8_t)m_config.keys.l_y.pos;
    }
    void setKeysLYPos(int val) {
        m_config.keys.l_y.pos = (uint8_t)val;
        emit keysLYPosUpdated();
    }
    int getKeysRXNeg() const {
        return (uint8_t)m_config.keys.r_x.neg;
    }
    void setKeysRXNeg(int val) {
        m_config.keys.r_x.neg = (uint8_t)val;
        emit keysRXNegUpdated();
    }
    int getKeysRXPos() const {
        return (uint8_t)m_config.keys.r_x.pos;
    }
    void setKeysRXPos(int val) {
        m_config.keys.r_x.pos = (uint8_t)val;
        emit keysRXPosUpdated();
    }
    int getKeysRYNeg() const {
        return (uint8_t)m_config.keys.r_y.neg;
    }
    void setKeysRYNeg(int val) {
        m_config.keys.r_y.neg = (uint8_t)val;
        emit keysRYNegUpdated();
    }
    int getKeysRYPos() const {
        return (uint8_t)m_config.keys.r_y.pos;
    }
    void setKeysRYPos(int val) {
        m_config.keys.r_y.pos = (uint8_t)val;
        emit keysRYPosUpdated();
    }
    int getDrumThreshold() const {
        return (uint8_t)m_config.drumThreshold;
    }
    void setDrumThreshold(int val) {
        m_config.drumThreshold = (uint8_t)val;
        emit drumThresholdUpdated();
    }
    QVariantMap getMidiType() const {
        QVariantMap l;
        for (auto pin: pins) {
            l[pin] = m_config.midi.type;
        }
        return l;
    }
    void setMidiTypeValueAt(int i, int val) {
        m_config.midi.type[i] = (uint8_t)val;
        emit midiTypeUpdated();
    }
    void setMidiTypeValue(QString key, int val) {
        m_config.midi.type[pins.indexOf(key)] = (uint8_t)val;
        emit midiTypeUpdated();
    }
    QVariantMap getMidiNote() const {
        QVariantMap l;
        for (auto pin: pins) {
            l[pin] = m_config.midi.note;
        }
        return l;
    }
    void setMidiNoteValueAt(int i, int val) {
        m_config.midi.note[i] = (uint8_t)val;
        emit midiNoteUpdated();
    }
    void setMidiNoteValue(QString key, int val) {
        m_config.midi.note[pins.indexOf(key)] = (uint8_t)val;
        emit midiNoteUpdated();
    }
    QVariantMap getMidiChannel() const {
        QVariantMap l;
        for (auto pin: pins) {
            l[pin] = m_config.midi.channel;
        }
        return l;
    }
    void setMidiChannelValueAt(int i, int val) {
        m_config.midi.channel[i] = (uint8_t)val;
        emit midiChannelUpdated();
    }
    void setMidiChannelValue(QString key, int val) {
        m_config.midi.channel[pins.indexOf(key)] = (uint8_t)val;
        emit midiChannelUpdated();
    }
    void setLED(QString key, int color) {
        uint32_t ucolor = color;
        auto pin = pins.indexOf(key)+1;
        for (auto& led : m_config.leds) {
            // Either update the colour of the led if it is already in the map, or set the first unused led.
            if (led.pin == pin || led.pin == 0) {
                led.pin = pin;
                led.red = ucolor >> 16 & 0xff;
                led.green = ucolor >> 8 & 0xff;
                led.blue = ucolor & 0xff;
                emit ledsUpdated();
                return;
            }
        }
    }
    QVariantMap getLEDColours() {
        QVariantMap l;
        for (auto led : m_config.leds) {
            if (led.pin == 0) break;
            l[pins[led.pin-1]] = led.red << 16 | led.green << 8 | led.blue;
        }
        for (auto pin: pins) {
            l[pin] = l.value(pin,0);
        }
        return l;
    }
    QVariantList getLEDs() {
        QVariantList l;
        for (auto led : m_config.leds) {
            if (led.pin == 0) break;
            l << pins[led.pin-1];
        }
        return l;
    }
    void clearLED(QString key) {
        auto pin = pins.indexOf(key)+1;
        auto a = std::remove_if(std::begin(m_config.leds), std::end(m_config.leds), [pin](Led_t x) { return x.pin == pin; });
        Led_t empty = {0,0,0};
        std::fill(a, std::end(m_config.leds), empty);
        std::copy(std::begin(m_config.leds), std::end(m_config.leds), std::begin(m_config.leds));
        emit ledsUpdated();
    }
    void moveLED(int from, int to) {
        if (from == to)
            return;
        auto const b = std::begin(m_config.leds);
        if (from < to)
            std::rotate(b + from, b + from + 1, b + to + 1);
        else
            std::rotate(b + to, b + from, b + from + 1);
        emit ledsUpdated();
    }
signals:
    void mainInputTypeUpdated();
    void mainSubTypeUpdated();
    void mainTiltTypeUpdated();
    void mainUnusedUpdated();
    void mainFretLEDModeUpdated();
    void mainMapLeftJoystickToDPadUpdated();
    void mainMapStartSelectToHomeUpdated();
    void mainMapNunchukAccelToRightJoyUpdated();
    void mainSignatureUpdated();
    void mainVersionUpdated();
    void pinsUpUpdated();
    void pinsDownUpdated();
    void pinsLeftUpdated();
    void pinsRightUpdated();
    void pinsStartUpdated();
    void pinsBackUpdated();
    void pinsLeftStickUpdated();
    void pinsRightStickUpdated();
    void pinsLBUpdated();
    void pinsRBUpdated();
    void pinsHomeUpdated();
    void pinsCaptureUpdated();
    void pinsAUpdated();
    void pinsBUpdated();
    void pinsXUpdated();
    void pinsYUpdated();
    void pinsLtUpdated();
    void pinsLtInvertedUpdated();
    void pinsRtUpdated();
    void pinsRtInvertedUpdated();
    void pinsLXUpdated();
    void pinsLXInvertedUpdated();
    void pinsLYUpdated();
    void pinsLYInvertedUpdated();
    void pinsRXUpdated();
    void pinsRXInvertedUpdated();
    void pinsRYUpdated();
    void pinsRYInvertedUpdated();
    void axisTriggerThresholdUpdated();
    void axisJoyThresholdUpdated();
    void axisMpu6050OrientationUpdated();
    void axisTiltSensitivityUpdated();
    void keysUpUpdated();
    void keysDownUpdated();
    void keysLeftUpdated();
    void keysRightUpdated();
    void keysStartUpdated();
    void keysBackUpdated();
    void keysLeftStickUpdated();
    void keysRightStickUpdated();
    void keysLBUpdated();
    void keysRBUpdated();
    void keysHomeUpdated();
    void keysCaptureUpdated();
    void keysAUpdated();
    void keysBUpdated();
    void keysXUpdated();
    void keysYUpdated();
    void keysLtUpdated();
    void keysRtUpdated();
    void keysLXNegUpdated();
    void keysLXPosUpdated();
    void keysLYNegUpdated();
    void keysLYPosUpdated();
    void keysRXNegUpdated();
    void keysRXPosUpdated();
    void keysRYNegUpdated();
    void keysRYPosUpdated();
    void drumThresholdUpdated();
    void ledsUpdated();
    void midiTypeUpdated();
    void midiNoteUpdated();
    void midiChannelUpdated();

private:
    Configuration_t m_config;
    const static QStringList pins;
};