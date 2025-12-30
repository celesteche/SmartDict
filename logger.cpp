#include "logger.h"
#include <iostream>

// 静态成员初始化
QFile* Logger::logFile = nullptr;
QMutex Logger::mutex;

void Logger::install()
{
    QMutexLocker locker(&mutex);
    if (logFile) return;

    logFile = new QFile("app.log");
    // 以追加模式打开，如果不存在则创建
    if (logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qInstallMessageHandler(Logger::messageHandler);
    }
}

void Logger::uninstall()
{
    QMutexLocker locker(&mutex);
    if (logFile) {
        qInstallMessageHandler(nullptr);
        logFile->close();
        delete logFile;
        logFile = nullptr;
    }
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&mutex);
    if (!logFile) return;

    QString level;
    switch (type) {
    case QtDebugMsg:    level = "DEBUG"; break;
    case QtInfoMsg:     level = "INFO "; break;
    case QtWarningMsg:  level = "WARN "; break;
    case QtCriticalMsg: level = "CRIT "; break;
    case QtFatalMsg:    level = "FATAL"; break;
    }

    // 格式化日志内容：时间 [级别] 内容 (文件名:行号)
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString logLine = QString("%1 [%2] %3 (%4:%5)\n")
                          .arg(timestamp, level, msg)
                          .arg(context.file).arg(context.line);

    // 写入文件
    QTextStream out(logFile);
    out << logLine;
    out.flush(); // 确保立即写入磁盘

    // 同时在控制台也打印一份，方便调试
    std::cout << logLine.toLocal8Bit().constData();
}
