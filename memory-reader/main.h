#ifndef MAIN_H
#define MAIN_H

#endif // MAIN_H

#include <QFile>
#include <QTextStream>
#include <QThread>
#include <QObject>

#include <KAuth>

using namespace KAuth;

class MyHelper : public QObject
{
    Q_OBJECT
    public Q_SLOTS:
        ActionReply read(const QVariantMap& args);
        ActionReply write(const QVariantMap& args);
        ActionReply longaction(const QVariantMap& args);
};
