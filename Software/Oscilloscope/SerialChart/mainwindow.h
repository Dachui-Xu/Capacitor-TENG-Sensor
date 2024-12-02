#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTimer>
#include "chart.h"
#include "DataProcessingThread.h"
QT_BEGIN_NAMESPACE

struct DataPoint {
    qreal x;
    qreal value;
    QString name;
    DataPoint(qreal xVal, qreal val, const QString &n)
        : x(xVal), value(val), name(n) {}
};

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void stopDataProcessingThread();
    void startDataProcessingThread();
    void init();
    void updateChart();
    void setupColorTableWidget();
    void handleColorCellClicked(int clickedRow, int clickedColumn);
    void setupShortcutTable();

private slots:
    void on_pushButton_2_clicked(); //刷新

    void on_pushButton_clicked(); //打开串口


    //void processData(const QByteArray &data);
    void processData();
   // void processBuffer();  // 处理接收到的缓冲区数据
   // void handleSerialError(const QString &error);
    void onDataReady(const QString &type, qreal x, qreal value, const QString &name);

    void updateAxis(const QString &type);

    void on_clear_clicked();

    void on_start_clicked();

    void on_pause_clicked();

    void on_export_2_clicked();

    void on_autoAdjust_clicked();

    void on_manualAdjust_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serialPort;
    QByteArray buffer;  // 数据接收缓冲区
    // 添加 tengChart 和 capChart 成员变量
    Chart *tengChart;
    Chart *capChart;
    DataProcessingThread *dataProcessingThread;
    bool dataProcssingThreadFlag = true;
    QTimer *updateTimer ;
    // 缓存待绘制的数据
    QList<DataPoint> pendingTengData;
    QList<DataPoint> pendingCapData;
    QMap<QString, QLineSeries*> seriesMap;  // 保存曲线名称和曲线对象的映射

};

#endif // MAINWINDOW_H
