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

    // 初始化数据库
    bool initDatabase();

    // --- 表1：搜索历史 (history) ---
    void addHistory(const QString &word);
    QStringList getHistory();
    void clearHistory();
    // 新增：删除单条历史记录
    void deleteHistory(const QString &word);

    // --- 表2：翻译缓存 (dict_cache) ---
    void saveCache(const QString &word, const QString &result);
    QString getCache(const QString &word);

private:
    explicit DatabaseHelper(QObject *parent = nullptr);
    ~DatabaseHelper();
    DatabaseHelper(const DatabaseHelper&) = delete;
    DatabaseHelper& operator=(const DatabaseHelper&) = delete;

    QSqlDatabase m_db;
};

#endif // DATABASEHELPER_H
