#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QFutureWatcher> // 引入观察者，用于监控后台线程
#include "networkmanager.h"

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
    void on_searchButton_clicked();
    void on_historyListView_clicked(const QModelIndex &index);
    void on_exportButton_clicked();

    void handleTranslation(const QString &word, const QString &result);
    void handleError(const QString &errorMsg);

    // 新增：处理多线程导出完成后的回调
    void onExportFinished();

private:
    Ui::MainWindow *ui;
    QStringListModel *m_historyModel;
    NetworkManager *m_netManager;

    // 多线程观察者：负责监视后台导出任务
    QFutureWatcher<bool> m_exportWatcher;

    void updateHistoryView();
};
#endif // MAINWINDOW_H
