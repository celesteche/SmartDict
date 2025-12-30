#include "filehelper.h"

FileHelper::FileHelper(QObject *parent) : QObject(parent) {}

bool FileHelper::exportHistoryToFile(const QString &filePath, const QStringList &history)
{
    if (filePath.isEmpty()) return false;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << "--- SmartDict Search History ---" << "\n";
    out << "Export Date: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
    out << "--------------------------------" << "\n\n";

    for (const QString &word : history) {
        out << word << "\n";
    }

    file.close();
    return true;
}

// --- 新增：导入逻辑实现 ---
QStringList FileHelper::importHistoryFromFile(const QString &filePath)
{
    QStringList result;
    if (filePath.isEmpty()) return result;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return result;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        // 过滤掉空行和我们导出时生成的装饰行
        if (line.isEmpty() || line.startsWith("-") || line.startsWith("Export Date:")) {
            continue;
        }
        result << line;
    }

    file.close();
    return result;
}
