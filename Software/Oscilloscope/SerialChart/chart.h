#ifndef CHART_H
#define CHART_H

#include <QObject>
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#include <QSplineSeries>
#include <QLineSeries>
#include <QWheelEvent>

QT_BEGIN_NAMESPACE

#define MAX_AXISX 10
#define MAX_POINTS (MAX_AXISX)*50*3 + 100

class Chart : public QChartView
{
    Q_OBJECT
    Q_PROPERTY(QList<QLineSeries*> seriesList READ getSeriesList CONSTANT)
public:
    explicit  Chart(QWidget *parent = nullptr,
                const QString &chartTitle = "折线图",
                const QString &xAxisTitle = "X 轴",
                const QString &yAxisTitle = "Y 轴"
                );
    ~Chart(){ allCharts.removeAll(this);}
    void addPoint(qreal x,qreal y,const QString &seriesName);
    void clearData();
    void setXAxis(qreal min, qreal max);
    void setXAxisRange(qreal min, qreal max);
    void setYAxisRange(qreal min, qreal max);
    //void setSeriesColor(const QColor &color);
    // 生成随机曲线
    void generateRandomSeries(int pointCount, const QString &seriesName);
    // 动态调整坐标轴范围
    void adjustAxisRanges();
    QList<QColor> getSeriesColors() const;        // 获取所有曲线的颜色
    void setSeriesColor(const QString &seriesName, const QColor &color);  // 设置某条曲线的颜色
    QList<QLineSeries*> getSeriesList() const {
        return seriesList;
    }
    static const QList<Chart*>& getAllCharts();  // 获取所有实例化的 Chart 对象

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void addSeries();
public:
    static bool axisControlFlag; // 自动控制坐标轴变化
private:
    QChart *chart;
    QList<QLineSeries*> seriesList;  // 存储所有线条
    QValueAxis *axisX;
    QValueAxis *axisY;
    // 设置坐标轴范围
    bool isPanning;      // 用于记录是否正在进行平移
    QPoint lastMousePos; // 记录最后的鼠标位置
    static QList<Chart*> allCharts;  // 静态变量，用于存储所有 Chart 对象
};

#endif // CHART_H
