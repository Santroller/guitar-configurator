#pragma once
#include <QObject>
#include <QMetaEnum>
#include <QDebug>
#define JS_ENUM(v) Q_ENUM(v) Q_INVOKABLE QVariantMap get_##v##_entries() {return getEntries(QMetaEnum::fromType<v>());}
#define JS_ENUM_NO_FIX(v) Q_ENUM(v) Q_INVOKABLE QVariantMap get_##v##_entries() {return getEntriesNoFix(QMetaEnum::fromType<v>());}
class ArdwiinoDefines: public QObject {
    Q_OBJECT
public:
#include "submodules/Ardwiino/src/shared/config/defines.h"
#include "submodules/Ardwiino/src/shared/input/inputs/wii_ext.h"
#include "submodules/Ardwiino/src/shared/input/inputs/ps2_cnt.h"
    static ArdwiinoDefines* getInstance();
    static QString fixKey(QString string) {
          auto sentence = string.toLower().split("_");
          for(auto i = 0; i< sentence.size(); i++){
             sentence[i] = sentence[i][0].toUpper() + sentence[i].right(sentence[i].size()-1);
          }
          return sentence.join(" ").replace("Ps2","PS2").replace("Ps3","PS3").replace("Apa102", "APA102").replace("Midi","MIDI").replace("Dualshock","DualShock").replace("Udraw","uDraw");
    }
    static QVariantMap getEntries(QMetaEnum e) {
        QVariantMap map;
        for (int i = 0; i < e.keyCount(); i++) {
            map[fixKey(e.key(i))] = e.keyToValue(e.key(i));
        }
        return map;
    }
    static QVariantMap getEntriesNoFix(QMetaEnum e) {
        QVariantMap map;
        for (int i = 0; i < e.keyCount(); i++) {
            map[e.key(i)] = e.keyToValue(e.key(i));
        }
        return map;
    }
    JS_ENUM(TiltType)
    JS_ENUM(InputType)
    JS_ENUM(SubType)
    JS_ENUM(GyroOrientation)
    JS_ENUM(FirmwareType)
    JS_ENUM(FretLedMode)
    JS_ENUM(MidiType)
    JS_ENUM(WiiExtType)
    JS_ENUM(PsxControllerType)
    enum buttons {
        up,down,left,right,start,back,left_stick,right_stick,LB,RB,home,capture,a,b,x,y,lt,rt,l_x,l_y,r_x,r_y
    };
    JS_ENUM_NO_FIX(buttons)
    template <class T>
    static QString getName(T arg) {
        return fixKey(QMetaEnum::fromType<T>().valueToKey(arg));
    }
private:
    static ArdwiinoDefines* instance;

};
