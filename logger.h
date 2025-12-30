#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>

class Logger
{
public:
    // 安装全局日志处理器
    static void install();
    // 卸载日志处理器
    static void uninstall();

    // 核心处理函数：将 Qt 的调试信息重定向到文件
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    static QFile* logFile;
    static QMutex mutex; // 确保多线程下写日志安全
};

#endif // LOGGER_H
