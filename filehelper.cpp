#include "filehelper.h"

FileHelper::FileHelper(QObject *parent) : QObject(parent) {}

bool FileHelper::exportHistoryToFile(const QString &filePath, const QStringList &history)
{
    if (filePath.isEmpty()) return false;

    QFile file(filePath);
    // 以只写和文本模式打开文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    // 设置编码（Qt6 默认 UTF-8）
    out << "--- SmartDict Search History ---" << "\n";
    out << "Export Date: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
    out << "--------------------------------" << "\n\n";

    for (const QString &word : history) {
        out << word << "\n";
    }

    file.close();
    return true;
}
