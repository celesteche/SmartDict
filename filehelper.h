#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QObject>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class FileHelper : public QObject
{
    Q_OBJECT
public:
    explicit FileHelper(QObject *parent = nullptr);

    // 导出功能（已存在）
    static bool exportHistoryToFile(const QString &filePath, const QStringList &history);

    // 新增：从文件导入历史记录
    // 返回读取到的单词列表
    static QStringList importHistoryFromFile(const QString &filePath);
};

#endif // FILEHELPER_H
