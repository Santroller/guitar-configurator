#include "portscanner.h"
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QIcon>
#include <QDirIterator>
#include <QBitmap>
#include <QCoreApplication>
PortScanner::PortScanner(Programmer *programmer, QObject *parent) : QObject(parent), m_hasSelected(false), m_selected(nullptr), programmer(programmer)
{
    m_model.push_back(new Port());
    if (settings.contains("configMode")) {
        m_graphical = settings.value("configMode").toBool();
    } else {
        m_graphical = true;
    }
    setSelected(nullptr);
}
void PortScanner::addPort(const QSerialPortInfo& serialPortInfo) {
    if (programmer->getStatus() == Status::AVRDUDE) return;
    if (m_selected != nullptr && m_hasSelected) {
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
        m_selected->handleDisconnection(serialPortInfo);
    }
    m_model.erase(std::remove_if(m_model.begin(), m_model.end(), [serialPortInfo](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == serialPortInfo.systemLocation();}), m_model.end());
    if (m_model.length() == 0) {
        m_model.push_back(new Port());
    }
    emit modelChanged();
}
void PortScanner::setSelected(Port* port) {
    m_selected = port;
    if (port != nullptr) {
        m_hasSelected = true;
        for (auto process: m_process) {
            if (process) {
                process->kill();
            }
        }
        m_model.erase(std::remove_if(m_model.begin(), m_model.end(), [](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == "dfu";}),m_model.end());
        selectedChanged();
        hasSelectedChanged();
        return;
    }
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("binaries");
    int i = 0;
    for (board_t board: ArdwiinoLookup::boards) {
        if (board.protocol == "dfu") {
            m_process[i++] = nullptr;
            continue;
        }
        m_process[i] = new QProcess();
        m_process[i]->setWorkingDirectory(dir.path());
        connect(qApp, SIGNAL(aboutToQuit()), m_process[i], SLOT(terminate()));
        connect(m_process[i], static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &PortScanner::complete);
        m_process[i]->start(dir.filePath("dfu-programmer"), {board.processor, "get"});
        i++;
    }
    selectedChanged();
}
void PortScanner::complete(int exitCode, QProcess::ExitStatus exitStatus) {
    (void)exitCode;
    (void)exitStatus;
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("binaries");
    int i = 0;
    if (m_selected != nullptr) return;
    for (auto process: m_process) {
        if (process != nullptr) {
            if (process->exitCode() == exitCode && process->state() == process->NotRunning) {
                QObject::disconnect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &PortScanner::complete);
                QObject::disconnect(qApp, SIGNAL(aboutToQuit()), process, SLOT(terminate()));
                auto board = ArdwiinoLookup::boards[i];
                if (exitCode == 0) {
                    if(std::find_if(m_model.begin(), m_model.end(), [board](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == "dfu" && (dynamic_cast<Port*>(object))->getBoard().processor == board.processor;}) == m_model.end()) {
                        m_model.push_back(new Port(board));
                        m_model.erase(std::remove_if(m_model.begin(), m_model.end(), [](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == "searching";}),m_model.end());
                        emit modelChanged();
                    }
                } else {
                    m_model.erase(std::remove_if(m_model.begin(), m_model.end(), [board](QObject* object){return (dynamic_cast<Port*>(object))->getPort() == "dfu" && (dynamic_cast<Port*>(object))->getBoard().processor == board.processor;}),m_model.end());
                }
                m_process[i] = new QProcess();
                m_process[i]->setWorkingDirectory(dir.path());
                connect(qApp, SIGNAL(aboutToQuit()), m_process[i], SLOT(terminate()));
                connect(m_process[i], static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &PortScanner::complete);
                m_process[i]->start(dir.filePath("dfu-programmer"), {ArdwiinoLookup::boards[i].processor, "get"});
            }
        }
        i++;
    }

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
        p.write("1209 2882 ff");
        p.closeWriteChannel();
        p.waitForFinished();
    }
}
void PortScanner::clearImages() {
    images.clear();
}

void PortScanner::toggleGraphics() {
    m_graphical = !m_graphical;
    graphicalChanged();
    settings.setValue("configMode", m_graphical);
}
QVector<uchar> data;
int width;
int height;
QString PortScanner::findElement(QString base, int w, int h, int mouseX, int mouseY) {
    if (images.isEmpty()) {
        width = w;
        height = h;
        data.clear();
        data.resize(width*height);
        auto imageList = QDir(":/"+base+"/components").entryList();
        imageList.sort();
        QVector<QRgb> colorMap;
        colorMap.push_back(qRgba(0,0,0,0));
        colorMap.push_back(qRgba(255,255,255,255));
        for (auto image: imageList) {
            auto i = QIcon(":/"+base+"/components/"+image).pixmap(QSize(width,height)).toImage();
            i = i.convertToFormat(QImage::Format_Indexed8, colorMap, Qt::AutoColor);
            auto s2 = i.height() * i.width();
            auto s3 = data.data();
            auto s = i.bits();
            while (s2--) {
                if (*s != 0) {
                    *s3 = *s;
                }
                s3++;
                s++;
            }
            images.push_back(image);
            colorMap.push_front(qRgba(0,0,0,0));
        }
    }
    double x = (double)mouseX/w;
    double y = (double)mouseY/h;
    if (x >= 1 || y >= 1 || x < 0 || y < 0) return "";
    x*= width;
    y*=height;
    auto c = data[(int)y*width+(int)x];
    if (c != 0) {
        return "/"+base+"/components/"+images[c-1];
    }
    return "";
}
