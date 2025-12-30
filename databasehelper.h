#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QDateTime>
#include <QDebug>

class DatabaseHelper : public QObject
{
    Q_OBJECT
public:
    static DatabaseHelper& instance();

    // 初始化数据库（创建两张表：history 和 dict_cache）
    bool initDatabase();

    // --- 表1：搜索历史 (history) ---
    void addHistory(const QString &word);
    QStringList getHistory();
    void clearHistory();

    // --- 表2：翻译缓存 (dict_cache) ---
    // 存储翻译结果
    void saveCache(const QString &word, const QString &result);
    // 获取缓存的翻译结果，如果不存在返回空字符串
    QString getCache(const QString &word);

private:
    explicit DatabaseHelper(QObject *parent = nullptr);
    ~DatabaseHelper();
    DatabaseHelper(const DatabaseHelper&) = delete;
    DatabaseHelper& operator=(const DatabaseHelper&) = delete;

    QSqlDatabase m_db;
};

#endif // DATABASEHELPER_H
