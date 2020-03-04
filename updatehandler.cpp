#include "updatehandler.h"
#include "QCoreApplication"
#include "QProcess"

UpdateHandler::UpdateHandler(QObject *parent) : QObject(parent), latestVersion(-1),currentVersion(-1)
{
    nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished, this, &UpdateHandler::onResult);
    QUrl url("https://api.github.com/repos/sanjay900/guitar-configurator/releases/latest");
    nam->get(QNetworkRequest(url));
    this->currentVersion = QString(VERSION_NUMBER).remove('v').toFloat();
    emit updateInfoChanged();
}
QString UpdateHandler::getUpdateInfo() {
    auto ret = QString("Current Version: %1, Latest Version: %2").arg(this->currentVersion);
    if  (latestVersion < 0) {
        ret = ret.arg("Retrieving Version");
    } else {
        ret = ret.arg(this->latestVersion);
    }
    return ret;
}
void UpdateHandler::startUpdate() {
#if Q_OS_WIN
    auto dir = QDir(QCoreApplication::applicationDirPath());
    auto m_process = new QProcess();
    m_process->setWorkingDirectory(dir.path());
    connect(qApp, SIGNAL(aboutToQuit()), m_process, SLOT(terminate()));
    m_process->start(dir.filePath("maintenancetool.exe"), {"--updater"});
    m_process->waitForStarted();
    QCoreApplication::exit(-1);
#endif
    qDebug() << "Updating!";
}

bool UpdateHandler::hasUpdate() {
    return true;
//    return latestVersion > currentVersion;
}

void UpdateHandler::onResult(QNetworkReply *reply){
    if(reply->error() == QNetworkReply::NoError){

        QByteArray result = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(result);
        QJsonObject obj = jsonResponse.object();
        this->latestVersion = obj["tag_name"].toString().remove('v').toFloat();
        emit updateInfoChanged();
    } else {
        //TODO: Do we warn the user if we cant check for updates?
    }
    reply->deleteLater();
}
