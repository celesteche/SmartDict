#include "mainwindow.h"
#include "databasehelper.h" // 引用头文件

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 初始化数据库
    if (!DatabaseHelper::instance().initDatabase()) {
        return -1; // 如果数据库打不开，程序直接退出
    }

    MainWindow w;
    w.show();
    return a.exec();
}
