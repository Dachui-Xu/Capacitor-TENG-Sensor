#include "chart.h"
#include <QRandomGenerator>


bool Chart::axisControlFlag = true; // 静态成员初始化
QList<Chart*> Chart::allCharts; // 静态成员变量的定义
QColor getRandomColor(){ return QColor(rand() % 256, rand() % 256, rand() % 256); }

Chart::Chart(QWidget *parent, const QString &chartTitle, const QString &xAxisTitle, const QString &yAxisTitle)
    : QChartView(parent), chart(new QChart), axisX(new QValueAxis), axisY(new QValueAxis)
{
    this->setChart(chart);  // 关联图表
    this->setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::Antialiasing, true);
    setInteractive(true);  // 启用交互
    setFocusPolicy(Qt::StrongFocus);  // 响应键盘和鼠标事件

    // 设置坐标轴
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    // 设置一些默认属性
    chart->setTitle(chartTitle);
    axisX->setTitleText(xAxisTitle);
    axisY->setTitleText(yAxisTitle);

    // 添加到静态列表中
    allCharts.append(this);
}

const QList<Chart*>& Chart::getAllCharts() {
    return allCharts;
}

void Chart::addPoint(qreal x, qreal y, const QString &seriesName) {
    QLineSeries *lineSerise = nullptr;
    for(QLineSeries* series : seriesList){
        if(series->name() == seriesName){
            lineSerise = series;
            break;
        }
    }
    // 如果找不到该条线，则创建新的线
    if(!lineSerise){
        lineSerise = new QLineSeries;
        lineSerise->setName(seriesName);
        seriesList.append(lineSerise);
        chart->addSeries(lineSerise);
        //为新线条设置随机颜色
        lineSerise->setColor(getRandomColor());
        lineSerise->attachAxis(axisX);
        lineSerise->attachAxis(axisY);
        emit addSeries();
    }
    lineSerise->append(x,y);
    // 如果点的数量超过MAX_POINTS，则移除最早的点
    if (lineSerise->count() > MAX_POINTS) {
        lineSerise->remove(0);  // 移除第一个点（最早添加的点）
    }
}

void Chart::clearData(){
    for(QLineSeries* series : seriesList){
        series->clear();
    }
}


void Chart::setXAxisRange(qreal min, qreal max)
{
    axisX->setRange(min,max);
}

void Chart::setYAxisRange(qreal min, qreal max)
{
    axisY->setRange(min,max);
}


void Chart::generateRandomSeries(int pointCount, const QString &seriesName)
{
    QRandomGenerator *generator = QRandomGenerator::global();

    // 添加随机点
    for (int i = 0; i < pointCount; ++i) {
        qreal x = static_cast<qreal>(i);  // X 轴值递增
        qreal y = generator->bounded(static_cast<int>(-50), static_cast<int>(50));  // Y 值随机在 -50 到 50 之间
        addPoint(x, y,seriesName);

    }

    // 动态调整 X 和 Y 轴的范围
    adjustAxisRanges();
}

void Chart::adjustAxisRanges()
{ if (axisControlFlag) {
        qreal minX = std::numeric_limits<qreal>::max(), maxX = std::numeric_limits<qreal>::min();
        qreal minY = std::numeric_limits<qreal>::max(), maxY = std::numeric_limits<qreal>::min();

        // 获取当前x轴的显示区间
        qreal currentMinX = axisX->min();
        qreal currentMaxX = axisX->max();

        for (QLineSeries* series : seriesList) {
            for (const QPointF &point : series->points()) {
                // 只考虑当前x轴显示区间内的点
                if (point.x() >= currentMinX && point.x() <= currentMaxX) {
                    // 更新y轴的最小值和最大值
                    if (point.y() < minY) minY = point.y();
                    if (point.y() > maxY) maxY = point.y();
                }

                // 更新x轴的范围
                if (point.x() < minX) minX = point.x();
                if (point.x() > maxX) maxX = point.x();
            }
        }

        // 更新x轴范围，最多显示30个单位的区间
        if (maxX <= MAX_AXISX) {
            setXAxisRange(minX, maxX);
        } else {
            setXAxisRange(maxX - MAX_AXISX, maxX);
        }

        // 根据当前显示的x轴区间动态调整y轴范围
        // 假设y轴的显示范围应根据x轴的当前显示区间来动态计算
        qreal yMargin = (maxY - minY) * 0.1;  // 10% 的上下边距
        setYAxisRange(minY - yMargin, maxY + yMargin);
    }
}

QList<QColor> Chart::getSeriesColors() const {
    QList<QColor> colors;
    for (QLineSeries* series : seriesList) {
        colors.append(series->color());
    }
    return colors;
}

void Chart::setSeriesColor(const QString &seriesName, const QColor &color) {
    for (QLineSeries* series : seriesList) {
        if (series->name() == seriesName) {
            series->setColor(color);
            break;
        }
    }
}


void Chart::wheelEvent(QWheelEvent *event)
{
    qreal factor = 1.2; // 缩放因子
    if (event->angleDelta().y() > 0) {
        // 放大
        chart->zoom(factor);
    } else {
        // 缩小
        chart->zoom(1.0 / factor);
    }
    event->accept();
}

void Chart::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isPanning = true;
        lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QChartView::mousePressEvent(event);
}

void Chart::mouseMoveEvent(QMouseEvent *event)
{
    int factor = 5; // 缩放因子
    if (isPanning) {
        QPointF delta = chart->mapToValue(lastMousePos) - chart->mapToValue(event->pos());
        chart->scroll(factor*12*delta.x(), factor*delta.y());
        lastMousePos = event->pos();
    }
    QChartView::mouseMoveEvent(event);
}

void Chart::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isPanning = false;
        setCursor(Qt::ArrowCursor);
    }
    QChartView::mouseReleaseEvent(event);
}







