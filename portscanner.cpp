#include "portscanner.h"
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QIcon>
#include <QDirIterator>
PortScanner::PortScanner(Programmer *programmer, QObject *parent) : QObject(parent), m_selected(nullptr), programmer(programmer)
{
    m_model.push_back(new Port());
}
void PortScanner::addPort(const QSerialPortInfo& serialPortInfo) {
    if (m_selected != nullptr) {
        m_selected->handleConnection(serialPortInfo);
        programmer->program(m_selected);
        if (!programmer->getRestore()) {
            m_selected->handleConnection(serialPortInfo);
            return;
        }
    }
    auto port = new Port(serialPortInfo);
    if (port->getPort() == nullptr) return;
    m_model.erase(std::remove_if(m_model.begin(), m_model.end(), [](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == "searching";}),m_model.end());
    m_model.push_back(port);
    port->open(serialPortInfo);
    connect(port, &Port::descriptionChanged,this,&PortScanner::update);
    connect(port, &Port::typeChanged, this, &PortScanner::clearImages);
    emit modelChanged();
}
void PortScanner::update() {
    //For whatever reason, just updating the model is not enough to make the QML combobox update correctly. However, removing all items then adding them again does work.
    auto m = m_model;
    m_model.clear();
    m_model.push_back(new Port());
    emit modelChanged();
    m_model.clear();
    m_model << m;
    emit modelChanged();
}
void PortScanner::removePort(const QSerialPortInfo& serialPortInfo) {
    if (m_selected != nullptr) {
        //Pass through to selected, replace its scanning implementation
    }
    m_model.erase(std::remove_if(m_model.begin(), m_model.end(), [serialPortInfo](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == serialPortInfo.systemLocation();}), m_model.end());
    if (m_model.length() == 0) {
        m_model.push_back(new Port());
    }
    emit modelChanged();
}
void PortScanner::fixLinux() {
    QFile f("/sys/bus/usb/drivers/xpad/new_id");
    f.open(QFile::ReadOnly);
    QString s = QString(f.readAll());
    f.close();
    if(!s.contains("1209 2882")) {
        QProcess p;
        p.start("pkexec", {"tee", "-a", "/sys/bus/usb/drivers/xpad/new_id"});
        p.waitForStarted();
        p.write("1209 2882");
        p.closeWriteChannel();
        p.waitForFinished();
    }
}
void PortScanner::clearImages() {
    images.clear();
}
QStringList PortScanner::getImages(QString base) {
    auto images = QDir(":/"+base).entryList();
    images.sort();
    return images;
}

QString PortScanner::findElement(QString base, int width, int height, int mouseX, int mouseY) {
    if (images.isEmpty()) {
        auto imageList = QDir(":/"+base).entryList();
        imageList.sort();
        for (auto image: getImages(base+"/components")) {
            auto i = QIcon(":/"+base+"/components/"+image).pixmap(QSize(width,height)).toImage();
            images.push_back(QPair<QImage,QString>(i,image));
        }
    }
    for (auto& i: images) {
        if (i.first.pixel((double)mouseX/width*i.first.width(),(double)mouseY/height*i.first.height())) {
            return "/"+base+"/components/"+i.second;
        }
    }
    return "";
}
