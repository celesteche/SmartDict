#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QFutureWatcher>
#include <QMenu>
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
    void on_clearButton_clicked();
    void on_copyButton_clicked();
    void on_historyContextMenu(const QPoint &pos);

    // 新增：导入按钮槽函数
    void on_importButton_clicked();

    void handleTranslation(const QString &word, const QString &result);
    void handleError(const QString &errorMsg);
    void onExportFinished();
    // 新增：导入完成回调
    void onImportFinished();

private:
    Ui::MainWindow *ui;
    QStringListModel *m_historyModel;
    NetworkManager *m_netManager;
    QFutureWatcher<bool> m_exportWatcher;
    // 新增：导入监视器
    QFutureWatcher<QStringList> m_importWatcher;

    void updateHistoryView();
};
#endif // MAINWINDOW_H
