#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "databasehelper.h"
#include <QMessageBox> // 引入弹窗提示
#include "filehelper.h"
#include <QFileDialog> // 引入文件对话框

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

// 导出按钮的实现
void MainWindow::on_exportButton_clicked()
{
    // 1. 获取当前所有的历史记录
    QStringList history = DatabaseHelper::instance().getHistory();
    if (history.isEmpty()) {
        QMessageBox::warning(this, "提示", "当前没有历史记录可以导出。");
        return;
    }

    // 2. 弹出文件保存对话框
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    "导出历史记录",
                                                    "history_export.txt",
                                                    "文本文件 (*.txt)");

    if (filePath.isEmpty()) return;

    // 3. 执行导出
    if (FileHelper::exportHistoryToFile(filePath, history)) {
        QMessageBox::information(this, "成功", "历史记录已成功导出到：\n" + filePath);
    } else {
        QMessageBox::critical(this, "错误", "文件导出失败，请检查权限。");
    }
}
