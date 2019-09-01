#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSerialPortInfo>
#include <QQmlContext>
#include "portscanner.h"
#include "programmer.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("universal");
    QQmlApplicationEngine engine;
    auto* scanner = new PortScanner();
    auto* programmer = new Programmer();
    engine.rootContext()->setContextProperty("scanner", scanner);
    engine.rootContext()->setContextProperty("programmer", programmer);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return QGuiApplication::exec();
}
