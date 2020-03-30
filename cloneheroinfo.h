#ifndef CLONEHEROINFO_H
#define CLONEHEROINFO_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>

class CloneHeroInfo : public QObject
{
    Q_OBJECT
public:
    explicit CloneHeroInfo(QObject *parent = nullptr);
    QTimer *timer;
private slots:
 void fileDownloaded(QNetworkReply* pReply);
 void tick();

private:
 QNetworkAccessManager m_WebCtrl;
 QString binary;
 int pid;
 QString version;
 QMap<QString, QString> hashes;

};

#endif // CLONEHEROINFO_H
