#ifndef UPDATEHANDLER_H
#define UPDATEHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
class UpdateHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString updateInfo READ getUpdateInfo NOTIFY updateInfoChanged)
    Q_PROPERTY(bool hasUpdate READ hasUpdate NOTIFY updateInfoChanged)
public:
    explicit UpdateHandler(QObject *parent = nullptr);

signals:
    void updateInfoChanged();
public slots:
    void startUpdate();
    QString getUpdateInfo();
    bool hasUpdate();
private:
    void onResult(QNetworkReply *reply);
    QNetworkAccessManager* nam;
    float latestVersion;
    float currentVersion;
};

#endif // UPDATEHANDLER_H
