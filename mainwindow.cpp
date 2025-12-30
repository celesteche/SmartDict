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

    ui->historyListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->historyListView, &QListView::customContextMenuRequested, this, &MainWindow::on_historyContextMenu);

    m_netManager = new NetworkManager(this);

    connect(m_netManager, &NetworkManager::translationFinished, this, &MainWindow::handleTranslation);
    connect(m_netManager, &NetworkManager::errorOccurred, this, &MainWindow::handleError);
    connect(&m_exportWatcher, &QFutureWatcher<bool>::finished, this, &MainWindow::onExportFinished);

    // --- 新增：连接导入监视器 ---
    connect(&m_importWatcher, &QFutureWatcher<QStringList>::finished, this, &MainWindow::onImportFinished);

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

void MainWindow::on_historyContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->historyListView->indexAt(pos);
    if (!index.isValid()) return;

    QString word = index.data().toString();

    QMenu menu(this);
    QAction *deleteAction = menu.addAction("删除此条记录");
    QAction *searchAction = menu.addAction("查询此单词");

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

// --- 新增：导入按钮点击处理 ---
void MainWindow::on_importButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择导入文件", "", "文本文件 (*.txt)");
    if (filePath.isEmpty()) return;

    ui->importButton->setEnabled(false);
    ui->statusbar->showMessage("后台导入中，请稍候...");

    // 使用异步线程读取文件
    QFuture<QStringList> future = QtConcurrent::run(&FileHelper::importHistoryFromFile, filePath);
    m_importWatcher.setFuture(future);
}

// --- 新增：导入完成后的处理 ---
void MainWindow::onImportFinished()
{
    QStringList words = m_importWatcher.result();
    ui->importButton->setEnabled(true);

    if (words.isEmpty()) {
        QMessageBox::warning(this, "提示", "未从文件中读取到有效单词。");
        ui->statusbar->showMessage("导入失败或文件为空", 3000);
        return;
    }

    // 将读取到的单词存入数据库
    for (const QString &word : words) {
        DatabaseHelper::instance().addHistory(word);
    }

    updateHistoryView();
    ui->statusbar->showMessage(QString("成功导入 %1 条记录").arg(words.size()), 3000);
    QMessageBox::information(this, "成功", QString("已成功导入 %1 条历史记录！").arg(words.size()));
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


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "关于 SmartDict",
                       "<h3>SmartDict - 智能词典 v1.0</h3>"
                       "<p>本软件由计算机科学与技术学院学生开发。</p>"
                       "<p><b>功能特性：</b></p>"
                       "<ul>"
                       "<li>基于 MyMemory API 的实时网络翻译</li>"
                       "<li>SQLite 本地缓存，提升二次查询速度</li>"
                       "<li>Model/View 架构管理搜索历史</li>"
                       "<li>支持历史记录的导入与导出 (TXT)</li>"
                       "<li>多线程异步操作，界面永不卡顿</li>"
                       "</ul>"
                       "<p>学号：2023414300104<br>姓名：陈熳墁</p>");
}
