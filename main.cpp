#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSerialPortInfo>
#include <QQmlContext>
#include "portscanner.h"
#include "programmer.h"
#include "status.h"
#include "input_types.h"
#include "joy_types.h"
#include "controllers.h"
#include "updatehandler.h"
#include "submodules/Ardwiino/src/shared/config/config.h"
#ifdef Q_OS_WIN
#include "winserialhotplug.h"
#else
#include "unixserialhotplug.h"
#endif

auto main(int argc, char *argv[]) -> int
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("universal");
    QQmlApplicationEngine engine;
    qmlRegisterUncreatableType<Status>("net.tangentmc", 1, 0, "Status",
                                       "Not creatable as it is an enum type.");
    qRegisterMetaType<InputTypes::Value>("Status::Value");
    qmlRegisterUncreatableType<InputTypes>("net.tangentmc", 1, 0, "InputTypes",
                                           "Not creatable as it is an enum type.");
    qRegisterMetaType<InputTypes::Value>("InputTypes::Value");
    qmlRegisterUncreatableType<Controllers>("net.tangentmc", 1, 0, "Controllers",
                                            "Not creatable as it is an enum type.");
    qRegisterMetaType<Controllers::Value>("Controllers::Value");
    qmlRegisterUncreatableType<TiltTypes>("net.tangentmc", 1, 0, "TiltTypes",
                                          "Not creatable as it is an enum type.");
    qRegisterMetaType<TiltTypes::Value>("TiltTypes::Value");
    qmlRegisterUncreatableType<MPU6050Orientations>("net.tangentmc", 1, 0, "MPU6050Orientations",
                                                    "Not creatable as it is an enum type.");
    qRegisterMetaType<MPU6050Orientations::Value>("MPU6050Orientations::Value");
    qmlRegisterUncreatableType<JoyTypes>("net.tangentmc", 1, 0, "JoyTypes",
                                         "Not creatable as it is an enum type.");
    qRegisterMetaType<JoyTypes::Value>("JoyTypes::Value");
    qmlRegisterSingletonType("net.tangentmc", 1, 0, "ArdwiinoLookup", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QJSValue {
        Q_UNUSED(engine)
        return scriptEngine->newQObject(ArdwiinoLookup::getInstance());
    });
    auto* programmer = new Programmer();
    auto* scanner = new PortScanner(programmer);
    auto* updates = new UpdateHandler();
#ifdef Q_OS_WIN
    WinSerialHotplug* filter = new WinSerialHotplug(scanner);
    app.installNativeEventFilter(filter);
#else
    new UnixSerialHotplug(scanner);
#endif
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
