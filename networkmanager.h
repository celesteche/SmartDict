#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);

    // 发起翻译请求
    void translateWord(const QString &word);

signals:
    // 当翻译完成时发射此信号，传回结果
    void translationFinished(const QString &word, const QString &result);
    // 当出错时发射此信号
    void errorOccurred(const QString &errorMsg);

private slots:
    // 处理网络响应内部槽函数
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
};

#endif // NETWORKMANAGER_H
