#include "networkmanager.h"

NetworkManager::NetworkManager(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    // 关联管理器完成信号到我们的处理槽
    connect(m_manager, &QNetworkAccessManager::finished, this, &NetworkManager::onReplyFinished);
}

void NetworkManager::translateWord(const QString &word)
{
    if (word.isEmpty()) return;

    // 使用 MyMemory 公共 API (英译汉)
    // 格式: https://api.mymemory.translated.net/get?q=WORD&langpair=en|zh-CN
    QString url = QString("https://api.mymemory.translated.net/get?q=%1&langpair=en|zh-CN").arg(word);

    QNetworkRequest request;
    request.setUrl(QUrl(url));

    // 发起 GET 请求
    m_manager->get(request);
}

void NetworkManager::onReplyFinished(QNetworkReply *reply)
{
    // 获取请求时的单词（从 URL 中提取）
    QString urlString = reply->url().toString();
    // 简单提取 q= 之后的单词
    QString word = urlString.split("q=").last().split("&").first();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred("网络请求失败: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    // 读取返回的 JSON 数据
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isNull() && doc.isObject()) {
        QJsonObject obj = doc.object();
        // 解析 MyMemory API 的特定 JSON 结构
        // 结构通常是: {"responseData": {"translatedText": "结果"}}
        if (obj.contains("responseData")) {
            QString result = obj["responseData"].toObject()["translatedText"].toString();
            emit translationFinished(word, result);
        } else {
            emit errorOccurred("解析结果失败");
        }
    }

    reply->deleteLater();
}
