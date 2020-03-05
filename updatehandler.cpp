#include "updatehandler.h"
#include "QCoreApplication"
#include "QProcess"
#include "QDir"

UpdateHandler::UpdateHandler(QObject *parent) : QObject(parent), latestVersion(-1),currentVersion(-1)
{
    nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished, this, &UpdateHandler::onResult);
    QUrl url("https://api.github.com/repos/sanjay900/guitar-configurator/releases/latest");
    nam->get(QNetworkRequest(url));
    this->currentVersion = QVersionNumber::fromString(QString(VERSION_NUMBER).remove('v'), NULL);
    emit updateInfoChanged();
}
QString UpdateHandler::getUpdateInfo() {
    auto ret = QString("Current Version: v%1, Latest Version: v%2").arg(this->currentVersion.toString());
    if  (latestVersion < QVersionNumber(0)) {
        ret = ret.arg("Retrieving Version");
    } else {
        ret = ret.arg(this->latestVersion.toString());
    }
    return ret;
}
void UpdateHandler::startUpdate() {
#ifdef Q_OS_WIN
    auto dir = QDir(QCoreApplication::applicationDirPath());
    auto m_process = new QProcess();
    m_process->setWorkingDirectory(dir.path());
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
        this->latestVersion = QVersionNumber::fromString(obj["tag_name"].toString().remove('v'));
        emit updateInfoChanged();
    } else {
        //TODO: Do we warn the user if we cant check for updates?
    }
    reply->deleteLater();
}
