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

    // 将历史记录列表保存到指定路径的文件中
    // 返回值：是否保存成功
    static bool exportHistoryToFile(const QString &filePath, const QStringList &history);
};

#endif // FILEHELPER_H
