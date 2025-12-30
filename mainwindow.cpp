#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "databasehelper.h" // 引用数据库类

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. 初始化 Model
    m_historyModel = new QStringListModel(this);

    // 2. 将 Model 绑定到 View (historyListView)
    ui->historyListView->setModel(m_historyModel);

    // 3. 启动时加载一次历史记录
    updateHistoryView();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateHistoryView()
{
    // 从数据库获取字符串列表
    QStringList history = DatabaseHelper::instance().getHistory();

    // 将数据设置给 Model，View 会自动刷新显示
    m_historyModel->setStringList(history);
}

void MainWindow::on_searchButton_clicked()
{
    QString word = ui->searchLineEdit->text().trimmed();
    if (word.isEmpty()) {
        return;
    }

    // 1. 将单词存入数据库
    DatabaseHelper::instance().addHistory(word);

    // 2. 刷新界面上的历史列表
    updateHistoryView();

    // 3. 清空输入框并显示提示（后续会在这里调用网络翻译）
    ui->searchLineEdit->clear();
    ui->resultBrowser->append(QString("正在查询单词: %1 ...").arg(word));
}

void MainWindow::on_historyListView_clicked(const QModelIndex &index)
{
    // 获取点击的单词
    QString word = index.data().toString();
    ui->searchLineEdit->setText(word);
    // 自动触发查询
    on_searchButton_clicked();
}
