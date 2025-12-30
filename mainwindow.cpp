#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "databasehelper.h"
#include "filehelper.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QtConcurrent> // 引入并发模块

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_historyModel = new QStringListModel(this);
    ui->historyListView->setModel(m_historyModel);

    m_netManager = new NetworkManager(this);

    connect(m_netManager, &NetworkManager::translationFinished, this, &MainWindow::handleTranslation);
    connect(m_netManager, &NetworkManager::errorOccurred, this, &MainWindow::handleError);

    // 连接多线程观察者的信号：当后台任务结束时，通知主线程
    connect(&m_exportWatcher, &QFutureWatcher<bool>::finished, this, &MainWindow::onExportFinished);

    updateHistoryView();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateHistoryView()
{
    QStringList history = DatabaseHelper::instance().getHistory();
    m_historyModel->setStringList(history);
}

void MainWindow::on_searchButton_clicked()
{
    QString word = ui->searchLineEdit->text().trimmed();
    if (word.isEmpty()) return;

    DatabaseHelper::instance().addHistory(word);
    updateHistoryView();

    QString cachedResult = DatabaseHelper::instance().getCache(word);

    if (!cachedResult.isEmpty()) {
        ui->resultBrowser->setHtml(QString("<b>[本地缓存]</b><br><br>%1").arg(cachedResult));
        ui->statusbar->showMessage("从本地缓存读取成功", 3000);
    } else {
        ui->resultBrowser->setText("正在联网查询中...");
        m_netManager->translateWord(word);
        ui->statusbar->showMessage("正在发起网络请求...", 3000);
    }
}

void MainWindow::handleTranslation(const QString &word, const QString &result)
{
    ui->resultBrowser->setHtml(QString("<b>[网络查询结果]</b><br><br>%1").arg(result));
    DatabaseHelper::instance().saveCache(word, result);
    ui->statusbar->showMessage("查询完成并已存入缓存", 3000);
}

void MainWindow::handleError(const QString &errorMsg)
{
    ui->resultBrowser->append(QString("\n<font color='red'>错误: %1</font>").arg(errorMsg));
    ui->statusbar->showMessage("查询出错", 3000);
}

void MainWindow::on_historyListView_clicked(const QModelIndex &index)
{
    QString word = index.data().toString();
    ui->searchLineEdit->setText(word);
    on_searchButton_clicked();
}

// 修改后的导出函数：使用多线程
void MainWindow::on_exportButton_clicked()
{
    QStringList history = DatabaseHelper::instance().getHistory();
    if (history.isEmpty()) {
        QMessageBox::warning(this, "提示", "当前没有历史记录可以导出。");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, "导出历史记录", "history_export.txt", "文本文件 (*.txt)");
    if (filePath.isEmpty()) return;

    // 禁用按钮，防止重复点击
    ui->exportButton->setEnabled(false);
    ui->statusbar->showMessage("后台导出中，请稍候...");

    // 使用 QtConcurrent::run 在后台线程执行耗时的文件写入操作
    // QFuture<bool> future = QtConcurrent::run(函数名, 参数1, 参数2...);
    QFuture<bool> future = QtConcurrent::run(&FileHelper::exportHistoryToFile, filePath, history);

    // 让观察者开始监视这个 future
    m_exportWatcher.setFuture(future);
}

// 后台线程完成后的回调函数（在主线程执行）
void MainWindow::onExportFinished()
{
    // 获取后台函数的返回值
    bool success = m_exportWatcher.result();

    ui->exportButton->setEnabled(true);
    ui->statusbar->showMessage("导出操作已完成", 3000);

    if (success) {
        QMessageBox::information(this, "成功", "历史记录已通过后台线程成功导出！");
    } else {
        QMessageBox::critical(this, "错误", "后台导出失败。");
    }
}
