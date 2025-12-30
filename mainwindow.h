#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
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

    // 新增：导出按钮槽函数
    void on_exportButton_clicked();

    void handleTranslation(const QString &word, const QString &result);
    void handleError(const QString &errorMsg);

private:
    Ui::MainWindow *ui;
    QStringListModel *m_historyModel;
    NetworkManager *m_netManager;

    void updateHistoryView();
};
#endif // MAINWINDOW_H
