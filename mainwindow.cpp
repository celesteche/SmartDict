#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "databasehelper.h"
#include "filehelper.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QtConcurrent>
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_historyModel = new QStringListModel(this);
    ui->historyListView->setModel(m_historyModel);

    // --- 新增：设置右键菜单策略 ---
    ui->historyListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->historyListView, &QListView::customContextMenuRequested, this, &MainWindow::on_historyContextMenu);

    m_netManager = new NetworkManager(this);

    connect(m_netManager, &NetworkManager::translationFinished, this, &MainWindow::handleTranslation);
    connect(m_netManager, &NetworkManager::errorOccurred, this, &MainWindow::handleError);
    connect(&m_exportWatcher, &QFutureWatcher<bool>::finished, this, &MainWindow::onExportFinished);

    connect(ui->searchLineEdit, &QLineEdit::returnPressed, this, &MainWindow::on_searchButton_clicked);

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

// --- 新增：右键菜单实现 ---
void MainWindow::on_historyContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->historyListView->indexAt(pos);
    if (!index.isValid()) return;

    QString word = index.data().toString();

    QMenu menu(this);
    QAction *deleteAction = menu.addAction("删除此条记录");
    QAction *searchAction = menu.addAction("查询此单词");

    // 执行菜单并获取点击的动作
    QAction *selectedAction = menu.exec(ui->historyListView->mapToGlobal(pos));

    if (selectedAction == deleteAction) {
        DatabaseHelper::instance().deleteHistory(word);
        updateHistoryView();
        ui->statusbar->showMessage("已删除记录: " + word, 2000);
    } else if (selectedAction == searchAction) {
        ui->searchLineEdit->setText(word);
        on_searchButton_clicked();
    }
}

void MainWindow::on_exportButton_clicked()
{
    QStringList history = DatabaseHelper::instance().getHistory();
    if (history.isEmpty()) {
        QMessageBox::warning(this, "提示", "当前没有历史记录可以导出。");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, "导出历史记录", "history_export.txt", "文本文件 (*.txt)");
    if (filePath.isEmpty()) return;

    ui->exportButton->setEnabled(false);
    ui->statusbar->showMessage("后台导出中，请稍候...");

    QFuture<bool> future = QtConcurrent::run(&FileHelper::exportHistoryToFile, filePath, history);
    m_exportWatcher.setFuture(future);
}

void MainWindow::onExportFinished()
{
    bool success = m_exportWatcher.result();
    ui->exportButton->setEnabled(true);
    ui->statusbar->showMessage("导出操作已完成", 3000);

    if (success) {
        QMessageBox::information(this, "成功", "历史记录已通过后台线程成功导出！");
    } else {
        QMessageBox::critical(this, "错误", "后台导出失败。");
    }
}

void MainWindow::on_clearButton_clicked()
{
    if (m_historyModel->stringList().isEmpty()) return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认", "确定要清空所有搜索历史吗？\n(此操作不可撤销)",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        DatabaseHelper::instance().clearHistory();
        updateHistoryView();
        ui->statusbar->showMessage("搜索历史已清空", 3000);
    }
}

void MainWindow::on_copyButton_clicked()
{
    QString text = ui->resultBrowser->toPlainText();
    if (text.isEmpty()) {
        ui->statusbar->showMessage("没有内容可以复制", 2000);
        return;
    }

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);

    ui->statusbar->showMessage("已复制到剪贴板", 2000);
}
