#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include "networkmanager.h" // 引入网络管理类

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
    // UI 交互槽函数
    void on_searchButton_clicked();
    void on_historyListView_clicked(const QModelIndex &index);

    // 处理网络返回结果的槽函数
    void handleTranslation(const QString &word, const QString &result);
    void handleError(const QString &errorMsg);

private:
    Ui::MainWindow *ui;
    QStringListModel *m_historyModel;

    // 网络管理类实例
    NetworkManager *m_netManager;

    void updateHistoryView();
};
#endif // MAINWINDOW_H
