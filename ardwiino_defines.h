#pragma once
#include <QObject>
#include <QMetaEnum>
#define JS_ENUM(v) Q_ENUM(v) Q_INVOKABLE QVariantMap get_##v##_entries() {return getEntries(QMetaEnum::fromType<v>());}
class ArdwiinoDefines: public QObject {
    Q_OBJECT
public:
#include "submodules/Ardwiino/src/shared/config/defines.h"
    static ArdwiinoDefines* getInstance();
    static QString fixKey(QString string) {
          auto sentence = string.toLower().split("_");
          for(auto i = 0; i< sentence.size(); i++){
             sentence[i] = sentence[i][0].toUpper() + sentence[i].right(sentence[i].size()-1);
          }
          return sentence.join(" ").replace("Ps3","PS3").replace("Apa102", "APA102");
    }
    static QVariantMap getEntries(QMetaEnum e) {
        QVariantMap map;
        for (int i = 0; i < e.keyCount(); i++) {
            map[fixKey(e.key(i))] = e.keyToValue(e.key(i));
        }
        return map;
    }
    JS_ENUM(tilt)
    JS_ENUM(input)
    JS_ENUM(subtype)
    JS_ENUM(gyro)
    JS_ENUM(firmware_type)
    JS_ENUM(fret_mode)
    template <class T>
    static QString getName(T arg) {
        return fixKey(QMetaEnum::fromType<T>().valueToKey(arg));
    }
private:
    static ArdwiinoDefines* instance;

};
