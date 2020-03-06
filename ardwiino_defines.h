#pragma once
#include <QObject>
#include <QMetaEnum>
#define ENUM_A(v) Q_ENUM(v) Q_INVOKABLE QVariantMap get_##v##_entries() {return getEntries(QMetaEnum::fromType<v>());}
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
          return sentence.join(" ").replace("Ps3","PS3");
    }
    static QVariantMap getEntries(QMetaEnum e) {
        QVariantMap map;
        for (int i = 0; i < e.keyCount(); i++) {
            map[fixKey(e.key(i))] = e.keyToValue(e.key(i));
        }
        return map;
    }
    ENUM_A(tilt)
    ENUM_A(input)
    ENUM_A(subtype)
    ENUM_A(gyro)
    ENUM_A(firmware_type)
    ENUM_A(fret_mode)
    template <class T>
    static QString getName(T arg) {
        return fixKey(QMetaEnum::fromType<T>().valueToKey(arg));
    }
private:
    static ArdwiinoDefines* instance;

};
