#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSerialPortInfo>
#include <QQmlContext>
#include "portscanner.h"
#include "programmer.h"
#include "status.h"
#include "updatehandler.h"
#include "submodules/Ardwiino/src/shared/config/config.h"
#include "ardwiino_defines.h"
#include "cloneheroinfo.h"
#ifdef Q_OS_WIN
#include "winserialhotplug.h"
#else
#include "unixserialhotplug.h"
#include "elevatepermissionsunix.h"
#endif

auto main(int argc, char *argv[]) -> int
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("universal");
    QQmlApplicationEngine engine;
    qmlRegisterUncreatableType<Status>("net.tangentmc", 1, 0, "Status",
                                       "Not creatable as it is an enum type.");
    qRegisterMetaType<Status::Value>("Status::Value");
    qmlRegisterSingletonType("net.tangentmc", 1, 0, "ArdwiinoLookup", [](QQmlEngine*, QJSEngine *scriptEngine) -> QJSValue {
        return scriptEngine->newQObject(ArdwiinoLookup::getInstance());
    });
    qmlRegisterUncreatableType<ArdwiinoDefines>("net.tangentmc", 1, 0, "ArdwiinoDefinesValues","Not creatable as it is an enum type.");
    qmlRegisterSingletonType("net.tangentmc", 1, 0, "ArdwiinoDefines", [](QQmlEngine*, QJSEngine *scriptEngine) -> QJSValue {
        return scriptEngine->newQObject(ArdwiinoDefines::getInstance());
    });
    auto* programmer = new Programmer();
    auto* scanner = new PortScanner(programmer);
    auto* updates = new UpdateHandler();
#ifdef Q_OS_WIN
    WinSerialHotplug* filter = new WinSerialHotplug(scanner);
    app.installNativeEventFilter(filter);
#else
    new ElevatePermissionsUnix();
    new UnixSerialHotplug(scanner);
#endif
    new CloneHeroInfo();
    engine.rootContext()->setContextProperty("updateHandler", updates);
    engine.rootContext()->setContextProperty("scanner", scanner);
    engine.rootContext()->setContextProperty("programmer", programmer);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
#ifdef Q_OS_WIN
    QObject* m_rootObject = engine.rootObjects().first();
    QWindow *window = qobject_cast<QWindow *>(m_rootObject);
    WId wid = window->winId();
    filter->init(wid);
#endif
    return QGuiApplication::exec();
}
