#include "databasehelper.h"

DatabaseHelper& DatabaseHelper::instance() {
    static DatabaseHelper inst;
    return inst;
}

DatabaseHelper::DatabaseHelper(QObject *parent) : QObject(parent) {
    // 使用 SQLite 数据库
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("history.db"); // 数据库文件名
}

DatabaseHelper::~DatabaseHelper() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseHelper::initDatabase() {
    if (!m_db.open()) {
        qDebug() << "Error: Failed to connect database." << m_db.lastError();
        return false;
    }

    QSqlQuery query;
    // 创建历史记录表：id, 单词, 时间戳
    QString sql = "CREATE TABLE IF NOT EXISTS history ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "word TEXT UNIQUE, "
                  "timestamp DATETIME)";

    if (!query.exec(sql)) {
        qDebug() << "Error: Fail to create table." << query.lastError();
        return false;
    }
    return true;
}

void DatabaseHelper::addHistory(const QString &word) {
    if (word.isEmpty()) return;

    QSqlQuery query;
    // 使用 REPLACE INTO 确保单词重复时更新时间戳
    query.prepare("INSERT OR REPLACE INTO history (word, timestamp) VALUES (:word, :time)");
    query.bindValue(":word", word);
    query.bindValue(":time", QDateTime::currentDateTime());

    if (!query.exec()) {
        qDebug() << "Add History Error:" << query.lastError();
    }
}

QStringList DatabaseHelper::getHistory() {
    QStringList history;
    QSqlQuery query("SELECT word FROM history ORDER BY timestamp DESC LIMIT 50");

    while (query.next()) {
        history << query.value(0).toString();
    }
    return history;
}

void DatabaseHelper::clearHistory() {
    QSqlQuery query;
    query.exec("DELETE FROM history");
}
