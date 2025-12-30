#include "mainwindow.h"
#include "databasehelper.h"
#include "logger.h" // 引入日志类

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. 安装日志系统
    Logger::install();

    qDebug() << "--- SmartDict Application Started ---";

    // 2. 初始化数据库
    if (!DatabaseHelper::instance().initDatabase()) {
        qCritical() << "Database initialization failed!";
        return -1;
    }

    MainWindow w;
    w.show();

    int result = a.exec();

    qDebug() << "--- SmartDict Application Normal Exit ---";

    // 3. 程序结束前卸载日志
    Logger::uninstall();

    return result;
}
