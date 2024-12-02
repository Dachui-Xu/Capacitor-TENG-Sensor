#include "mainwindow.h"
#include "chart.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置窗口标题为 "TENG&Cap"
    setWindowTitle("TENG&Cap");
    setWindowIcon(QIcon(":/icon/icon/Capactive.ico"));

    init();
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->comboBox->clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
        QString portDescription = QString("%1 (%2)")
                                    .arg(info.portName())       // 串口名称，如 "COM3"
                                    .arg(info.description());   // 串口描述，如 "USB-SERIAL CH340"
        ui->comboBox->addItem(portDescription,info.portName());
    }
}

void MainWindow::on_pushButton_clicked()
{
    ui->start->setEnabled(false);
    if(ui->pushButton->text() == tr("Open Serial")) {
        // 打开串口
        serialPort->setPortName(ui->comboBox->currentData().toString());
        serialPort->setBaudRate(ui->comboBox_2->currentText().toInt());
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

        if (!serialPort->open(QIODevice::ReadWrite)) {
            QMessageBox::about(this, tr("Error"), tr("Failed to open the serial port"));
            return;
        }

        ui->comboBox->setEnabled(false);
        ui->comboBox_2->setEnabled(false);
        ui->export_2->setEnabled(true);
        ui->clear->setEnabled(true);
        ui->pushButton->setText(tr("Close Serial"));
        ui->pushButton->setShortcut(QKeySequence("shift+space"));
        connect(serialPort, SIGNAL(readyRead()), this, SLOT(processData()));  // 确保串口打开时已连接信号槽
    } else {
        // 关闭串口
        stopDataProcessingThread();  // 首先停止数据处理线程
        if (serialPort->isOpen()) {
            serialPort->close();  // 关闭串口
        }
        ui->comboBox->setEnabled(true);
        ui->comboBox_2->setEnabled(true);
        ui->pushButton->setText(tr("Open Serial"));

        // 断开信号槽连接，确保不会在串口关闭时触发读操作
        disconnect(serialPort, SIGNAL(readyRead()), this, SLOT(processData()));
    }
}

void MainWindow::processData()
{
    QByteArray data = serialPort->readAll();
    // 处理接收到的数据，更新UI或图表
    ui->textBrowser->insertPlainText(data);
    ui->textBrowser->moveCursor(QTextCursor::End);
    if (!data.isEmpty()&&dataProcssingThreadFlag) {
        // 将接收到的数据添加到缓冲区
        dataProcessingThread->setData(data);
    }

}

void MainWindow::onDataReady(const QString &type, qreal x, qreal value, const QString &name)
{
    if (type == "teng") {
        tengChart->addPoint(x, value, name);  // 添加 ADC 数据点
    } else if (type == "cap") {
        capChart->addPoint(x, value, name);  // 添加 CAP 数据点
    }
}

void MainWindow::updateAxis(const QString &type)
{
    if(type == "teng"){
        tengChart->adjustAxisRanges();
    }
    if(type == "cap"){
        capChart->adjustAxisRanges();
    }
}



void MainWindow::on_clear_clicked()
{
    tengChart->clearData();
    capChart->clearData();
    stopDataProcessingThread();
    ui->start->setEnabled(true);
    ui->pause->setText("Pause");
    ui->pause->setEnabled(false);
    ui->textBrowser->clear();
    disconnect(serialPort, SIGNAL(readyRead()), this, SLOT(processData()));
    ui->export_2->setEnabled(false);
    ui->start->setEnabled(true);
}


void MainWindow::on_start_clicked()
{
    if(dataProcessingThread!=nullptr) return; // 避免重复建立
    startDataProcessingThread();
    ui->pause->setEnabled(true);
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(processData()));
    ui->export_2->setEnabled(true);
}

void MainWindow::stopDataProcessingThread()
{
    if (dataProcessingThread && dataProcessingThread->isRunning()) {
        // 发送停止信号，并请求线程退出
        dataProcessingThread->stop();
        dataProcessingThread->quit();  // 请求退出
        dataProcessingThread->wait();  // 等待线程结束
        dataProcssingThreadFlag = false;
        // 清理工作，确保线程已完全停止
        delete dataProcessingThread;  // 删除线程对象
        dataProcessingThread = nullptr;  // 设置指针为 nullptr，防止悬空指针
    }
}

void MainWindow::startDataProcessingThread()
{
    // 创建并启动线程
    dataProcessingThread = new DataProcessingThread(this);
    connect(dataProcessingThread, &DataProcessingThread::dataReady, this, &MainWindow::onDataReady);
    /* 坐标轴更更新信号槽 */
    connect(dataProcessingThread, &DataProcessingThread::updateAxis,this,&MainWindow::updateAxis);
    dataProcessingThread->start();
    dataProcssingThreadFlag = true;
}

void MainWindow::init()
{
    /* 串口 */
    serialPort = new QSerialPort(this);
    // 创建并启动线程
    startDataProcessingThread();
    on_pushButton_2_clicked();
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(processData()));
    dataProcessingThread->start();
    /* 绘图 */  
    tengChart = new Chart(ui->teng, "TENG", "Time (s)", "Voltage (mV)");
    capChart = new Chart(ui->teng, "CAP", "Time (s)", "Capacitance (pF)");

    // tengChart 放到约束里
    ui->verticalLayout_2->addWidget(tengChart);
    // capChart 放到约束里
    ui->verticalLayout_9->addWidget(capChart);

    /* 界面设置 */
    ui->export_2->setEnabled(false);
    ui->start->setEnabled(false);
    ui->clear->setEnabled(false);
    dataProcessingThread->setPriority(QThread::HighPriority);
    setupShortcutTable();

    /* 更新线，更新颜色表格 */
    connect(capChart,&Chart::addSeries,this,&MainWindow::setupColorTableWidget);
    connect(tengChart,&Chart::addSeries,this,&MainWindow::setupColorTableWidget);
}

void MainWindow::setupShortcutTable() {
    ui->shortcutTableWidget->verticalHeader()->setVisible(false);
   // ui->shortcutTableWidget->horizontalHeader()->setVisible(false);
    // 设置快捷键表格的列数和表头
    ui->shortcutTableWidget->setColumnCount(2);
    QStringList headers;
    headers << "Action" << "Shortcut";
    ui->shortcutTableWidget->setHorizontalHeaderLabels(headers);

    // 设置快捷键表格的行数
    ui->shortcutTableWidget->setRowCount(9);

    // 设置每一行的操作名称和对应的快捷键
    ui->shortcutTableWidget->setItem(0, 0, new QTableWidgetItem("Open Serial"));
    ui->shortcutTableWidget->setItem(0, 1, new QTableWidgetItem("Shift + Space"));

    ui->shortcutTableWidget->setItem(1, 0, new QTableWidgetItem("Close Serial"));
    ui->shortcutTableWidget->setItem(1, 1, new QTableWidgetItem("Shift + Space"));

    ui->shortcutTableWidget->setItem(2, 0, new QTableWidgetItem("Export"));
    ui->shortcutTableWidget->setItem(2, 1, new QTableWidgetItem("Shift + E"));

    ui->shortcutTableWidget->setItem(3, 0, new QTableWidgetItem("Start"));
    ui->shortcutTableWidget->setItem(3, 1, new QTableWidgetItem("Shift + S"));

    ui->shortcutTableWidget->setItem(4, 0, new QTableWidgetItem("Clear"));
    ui->shortcutTableWidget->setItem(4, 1, new QTableWidgetItem("Shift + C"));

    ui->shortcutTableWidget->setItem(5, 0, new QTableWidgetItem("Pause"));
    ui->shortcutTableWidget->setItem(5, 1, new QTableWidgetItem("Shift + P"));

    ui->shortcutTableWidget->setItem(6, 0, new QTableWidgetItem("Resume"));
    ui->shortcutTableWidget->setItem(6, 1, new QTableWidgetItem("Shift + R"));

    ui->shortcutTableWidget->setItem(7, 0, new QTableWidgetItem("Automatic Adjustment"));
    ui->shortcutTableWidget->setItem(7, 1, new QTableWidgetItem("Shift + A"));

    ui->shortcutTableWidget->setItem(8, 0, new QTableWidgetItem("Manual Adjustment"));
    ui->shortcutTableWidget->setItem(8, 1, new QTableWidgetItem("Shift + M"));

    // 调整列宽，使其自适应内容
    ui->shortcutTableWidget->resizeColumnsToContents();

    // 使最后一列自动扩展填充剩余的空间
    ui->shortcutTableWidget->horizontalHeader()->setStretchLastSection(true);

    // 设置所有列均匀填充整个表格的宽度
    ui->shortcutTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}


void MainWindow::setupColorTableWidget() {
    ui->colorTableWidget->setColumnCount(2);
    QStringList headers;
    headers << "Channel" << "Color";
    ui->colorTableWidget->setHorizontalHeaderLabels(headers);

    // 清空 QTableWidget 并初始化行数
    ui->colorTableWidget->setRowCount(0);
    int row = 0;
    seriesMap.clear();  // 清空之前的映射

    for (Chart* chart : Chart::getAllCharts()) {
        for (QLineSeries* series : chart->getSeriesList()) {
            ui->colorTableWidget->insertRow(row);

            // 曲线名称
            QTableWidgetItem *nameItem = new QTableWidgetItem(series->name());
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
            ui->colorTableWidget->setItem(row, 0, nameItem);

            // 当前颜色
            QTableWidgetItem *colorItem = new QTableWidgetItem();
            colorItem->setBackground(series->color());
            ui->colorTableWidget->setItem(row, 1, colorItem);

            // 将曲线加入映射
            seriesMap.insert(series->name(), series);

            row++;
        }
    }

    // 处理颜色单元格的点击事件
    disconnect(ui->colorTableWidget, &QTableWidget::cellClicked, this, &MainWindow::handleColorCellClicked);
    connect(ui->colorTableWidget, &QTableWidget::cellClicked, this, &MainWindow::handleColorCellClicked);
}

void MainWindow::handleColorCellClicked(int clickedRow, int clickedColumn) {
    QString seriesName = ui->colorTableWidget->item(clickedRow, 0)->text();
    QLineSeries* series = seriesMap.value(seriesName, nullptr); // 查找曲线对象
    // 确保只在点击颜色列时处理
    if (clickedColumn == 1) {
        if (series) {
            QColor newColor = QColorDialog::getColor(series->color(), this, "Select Color");
            if (newColor.isValid() && newColor != series->color()) {
                series->setColor(newColor);
                ui->colorTableWidget->item(clickedRow, clickedColumn)->setBackground(newColor);
                // 取消选中状态，确保新颜色显示
                ui->colorTableWidget->clearSelection();
            }
        }
    }
     if (clickedColumn == 0) {
        // 切换曲线的显示状态
        if (series) {
            bool isVisible = series->isVisible();
            series->setVisible(!isVisible);

            // 修改表格中的曲线背景颜色，以反映当前的可见状态
            if (series->isVisible()) {
                ui->colorTableWidget->item(clickedRow, 0)->setBackground(Qt::white);
            } else {
                ui->colorTableWidget->item(clickedRow, 0)->setBackground(Qt::lightGray);
            }
        }
    }
}




void MainWindow::on_pause_clicked()
{   if(dataProcessingThread==nullptr){
        return;
    }
    if(ui->pause->text() == tr("Pause")){
        ui->pause->setText(tr("Resume"));
      //  dataProcessingThread->clearData();
        dataProcssingThreadFlag = false;
        ui->start->setEnabled(false);
        ui->pause->setShortcut(QKeySequence("Shift+R"));
        disconnect(serialPort, SIGNAL(readyRead()), this, SLOT(processData()));
    }else{
        ui->pause->setText(tr("Pause"));
      //  dataProcessingThread->clearData();
        dataProcssingThreadFlag = true;
        ui->start->setEnabled(true);
        connect(serialPort, SIGNAL(readyRead()), this, SLOT(processData()));
    }

}


void MainWindow::on_export_2_clicked() {
    // 获取当前时间，格式化为"yyyyMMdd_HHmmss"
    QString currentTime = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");

    // 设置两个单独的文件名
    QString capFilePath = QFileDialog::getSaveFileName(this, tr("Save CAP data as CSV file"),
                                                       QString("CAP_%1.csv").arg(currentTime),
                                                       "CSV Files (*.csv);;All Files (*)");
    QString tengFilePath = QFileDialog::getSaveFileName(this, tr("Save TENG data as CSV file"),
                                                        QString("TENG_%1.csv").arg(currentTime),
                                                        "CSV Files (*.csv);;All Files (*)");

    // 读取 textBrowser 的所有内容
    QString text = ui->textBrowser->toPlainText();
    QStringList lines = text.split('\n');

    // 如果用户选择了 CAP 文件路径，保存 CAP 数据
    if (!capFilePath.isEmpty()) {
        QFile capFile(capFilePath);
        if (!capFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"), tr("Unable to open CAP file for writing"));
        } else {
            QTextStream capStream(&capFile);
            // 写入 CAP 文件表头
            capStream << "cap0,cap1,cap2,cap3\n";
            // 遍历数据并写入 CAP 数据
            for (const QString &line : lines) {
                if (line.startsWith("CAP")) {
                    QString capData = line.mid(4);
                    capStream << capData << "\n";  // 保持逗号分隔的数据
                }
            }
            capFile.close();
        }
    }

    // 如果用户选择了 TENG 文件路径，保存 TENG 数据
    if (!tengFilePath.isEmpty()) {
        QFile tengFile(tengFilePath);
        if (!tengFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"), tr("Unable to open TENG file for writing"));
        } else {
            QTextStream tengStream(&tengFile);
            // 写入 TENG 文件表头
            tengStream << "teng0,teng1,teng2,teng3,teng4,teng5,teng6,teng7,teng8,teng9\n";
            // 遍历数据并写入 TENG 数据
            for (const QString &line : lines) {
                if (line.startsWith("ADC")) {
                    QString tengData = line.mid(4);
                    tengStream << tengData << "\n";  // 保持逗号分隔的数据
                }
            }
            tengFile.close();
        }
    }

    // 如果至少保存了一个文件，则弹出成功信息框
    if (!capFilePath.isEmpty() || !tengFilePath.isEmpty()) {
        QMessageBox::information(this, tr("Export Success!"), tr("Data has been successfully exported to CSV files!"));
    }
}

/*
 * @function 单选框：坐标轴自动调整
 **/
void MainWindow::on_autoAdjust_clicked()
{
    Chart::axisControlFlag = true;
}

/*
 * @function 单选框：坐标轴手动调整
 **/
void MainWindow::on_manualAdjust_clicked()
{
    Chart::axisControlFlag = false;
}

