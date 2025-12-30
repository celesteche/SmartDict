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
    // 单例模式：方便在程序任何地方调用数据库
    static DatabaseHelper& instance();

    // 初始化数据库（创建表）
    bool initDatabase();

    // 增加一条搜索记录
    void addHistory(const QString &word);

    // 获取所有搜索历史（按时间倒序）
    QStringList getHistory();

    // 清空历史记录（预留功能）
    void clearHistory();

private:
    explicit DatabaseHelper(QObject *parent = nullptr);
    ~DatabaseHelper();
    DatabaseHelper(const DatabaseHelper&) = delete;
    DatabaseHelper& operator=(const DatabaseHelper&) = delete;

    QSqlDatabase m_db;
};

#endif // DATABASEHELPER_H
