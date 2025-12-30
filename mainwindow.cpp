#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "databasehelper.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. 初始化 Model
    m_historyModel = new QStringListModel(this);
    ui->historyListView->setModel(m_historyModel);

    // 2. 初始化网络管理类
    m_netManager = new NetworkManager(this);

    // 3. 连接网络信号与槽
    connect(m_netManager, &NetworkManager::translationFinished, this, &MainWindow::handleTranslation);
    connect(m_netManager, &NetworkManager::errorOccurred, this, &MainWindow::handleError);

    // 4. 启动时加载历史记录
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

    // A. 记录到搜索历史表
    DatabaseHelper::instance().addHistory(word);
    updateHistoryView();

    // B. 缓存优先逻辑：先查本地数据库缓存表
    QString cachedResult = DatabaseHelper::instance().getCache(word);

    if (!cachedResult.isEmpty()) {
        // 如果缓存中有，直接显示，并在状态栏提示
        ui->resultBrowser->setHtml(QString("<b>[本地缓存]</b><br><br>%1").arg(cachedResult));
        ui->statusbar->showMessage("从本地缓存读取成功", 3000);
    } else {
        // 如果缓存没有，发起网络请求
        ui->resultBrowser->setText("正在联网查询中...");
        m_netManager->translateWord(word);
        ui->statusbar->showMessage("正在发起网络请求...", 3000);
    }
}

void MainWindow::handleTranslation(const QString &word, const QString &result)
{
    // 1. 显示结果
    ui->resultBrowser->setHtml(QString("<b>[网络查询结果]</b><br><br>%1").arg(result));

    // 2. 存入本地缓存表，下次查询就快了
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
