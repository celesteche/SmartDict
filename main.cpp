#include "mainwindow.h"
#include "databasehelper.h"
#include "logger.h"

#include <QApplication>
#include <QFile> // 引入文件类用于读取样式表

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. 安装日志系统
    Logger::install();

    qDebug() << "--- SmartDict Application Started ---";

    // 2. 加载 QSS 样式表
    // 注意：路径 ":/style.qss" 中的冒号表示从资源文件（qrc）中读取
    QFile qssFile(":/style.qss");
    if (qssFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(qssFile.readAll());
        a.setStyleSheet(styleSheet);
        qssFile.close();
        qDebug() << "Successfully loaded QSS stylesheet.";
    } else {
        qWarning() << "Failed to load QSS stylesheet.";
    }

    // 3. 初始化数据库
    if (!DatabaseHelper::instance().initDatabase()) {
        qCritical() << "Database initialization failed!";
        return -1;
    }

    MainWindow w;
    w.show();

    int result = a.exec();

    qDebug() << "--- SmartDict Application Normal Exit ---";

    // 4. 程序结束前卸载日志
    Logger::uninstall();

    return result;
}
