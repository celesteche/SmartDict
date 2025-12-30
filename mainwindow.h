#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel> // 引入 Model 模块

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 当点击查询按钮时触发
    void on_searchButton_clicked();

    // 当点击搜索历史列表中的某一项时触发（预留）
    void on_historyListView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    // Model/View 框架的核心：模型
    QStringListModel *m_historyModel;

    // 私有辅助函数：刷新历史记录显示
    void updateHistoryView();
};
#endif // MAINWINDOW_H
