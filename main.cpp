#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSerialPortInfo>
#include <QQmlContext>
#include "portscanner.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    PortScanner* scanner = new PortScanner();
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("universal");
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("scanner", scanner);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
