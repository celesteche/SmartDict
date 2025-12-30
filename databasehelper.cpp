#include "databasehelper.h"

DatabaseHelper& DatabaseHelper::instance() {
    static DatabaseHelper inst;
    return inst;
}

DatabaseHelper::DatabaseHelper(QObject *parent) : QObject(parent) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("history.db");
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

    // 表1：搜索历史
    QString sqlHistory = "CREATE TABLE IF NOT EXISTS history ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "word TEXT UNIQUE, "
                         "timestamp DATETIME)";
    if (!query.exec(sqlHistory)) {
        qDebug() << "Error: Fail to create history table." << query.lastError();
    }

    // 表2：翻译缓存 (存储单词及其对应的翻译结果)
    QString sqlCache = "CREATE TABLE IF NOT EXISTS dict_cache ("
                       "word TEXT PRIMARY KEY, "
                       "result TEXT, "
                       "timestamp DATETIME)";
    if (!query.exec(sqlCache)) {
        qDebug() << "Error: Fail to create cache table." << query.lastError();
        return false;
    }

    return true;
}

void DatabaseHelper::addHistory(const QString &word) {
    if (word.isEmpty()) return;
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO history (word, timestamp) VALUES (:word, :time)");
    query.bindValue(":word", word);
    query.bindValue(":time", QDateTime::currentDateTime());
    query.exec();
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

// --- 缓存表操作 ---

void DatabaseHelper::saveCache(const QString &word, const QString &result) {
    if (word.isEmpty() || result.isEmpty()) return;
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO dict_cache (word, result, timestamp) VALUES (:word, :result, :time)");
    query.bindValue(":word", word);
    query.bindValue(":result", result);
    query.bindValue(":time", QDateTime::currentDateTime());
    if (!query.exec()) {
        qDebug() << "Save Cache Error:" << query.lastError();
    }
}

QString DatabaseHelper::getCache(const QString &word) {
    QSqlQuery query;
    query.prepare("SELECT result FROM dict_cache WHERE word = :word");
    query.bindValue(":word", word);
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString(); // 返回空字符串表示没找到
}
