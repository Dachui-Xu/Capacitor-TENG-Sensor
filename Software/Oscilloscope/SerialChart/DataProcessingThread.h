#ifndef DATAPROCESSINGTHREAD_H
#define DATAPROCESSINGTHREAD_H

#include <QThread>
#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include <QDebug>

#define MAX_BUFFER_SIZE 1024  // 设置缓冲区最大大小

class DataProcessingThread : public QThread {
    Q_OBJECT
public:
    explicit DataProcessingThread(QObject *parent = nullptr)
        : QThread(parent), stopThread(false) {}

    void stop() {
        QMutexLocker locker(&mutex);
        stopThread = true;
    }

    void setData(const QByteArray &newData) {
        QMutexLocker locker(&mutex);
        data.append(newData);
    }

    void clearData() {
        QMutexLocker locker(&mutex);
        data.clear();
        x_adc = 0;
        x_cap = 0;
    }

protected:
    void run() override {
        while (true) {
            {
                QMutexLocker locker(&mutex);
                if (stopThread) {
                    break;
                }
            }
            processDataInThread();
            msleep(20);  // 每 10 ms 处理一次
        }
    }

private:
    void processDataInThread() {
        QMutexLocker locker(&mutex);

        // 如果缓冲区大小超过最大限制，丢弃最早的数据
        if (data.size() > MAX_BUFFER_SIZE) {
            data.remove(0, data.size() - MAX_BUFFER_SIZE);
        }

        while (data.contains("\r\n")) {
            int endIndex = data.indexOf("\r\n");  // 找到完整数据的结束位置
            QByteArray completeData = data.left(endIndex);  // 提取完整数据
            data.remove(0, endIndex + 2);  // 移除已处理的部分
            qDebug() << "Buffer Size" << data.size();

            // 处理CAP或ADC数据
            QString dataStr = QString::fromUtf8(completeData);
            if (dataStr.startsWith("ADC")) {
                QStringList parts = dataStr.split(",");
                if (parts.size() > 1) {
                    for (int i = 1; i < parts.size(); ++i) {
                        int value = parts[i].toInt();
                        emit dataReady("teng", x_adc, value, "teng" + QString::number(i - 1));
                    }
                    emit updateAxis("teng");
                    x_adc += 0.02;  // 更新时间轴
                }
            } else if (dataStr.startsWith("CAP")) {
                QStringList parts = dataStr.split(",");
                if (parts.size() > 1) {
                    for (int i = 1; i < parts.size(); ++i) {
                        qreal value = parts[i].toDouble() / 1000;
                        emit dataReady("cap", x_cap, value, "cap" + QString::number(i - 1));
                    }
                    emit updateAxis("cap");
                    x_cap += 0.02;  // 更新时间轴
                }
            }
        }
    }

signals:
    void dataReady(const QString &type, qreal x, qreal value, const QString &name);
    void updateAxis(const QString &type);

private:
    bool stopThread;  // 用于控制线程停止
    QByteArray data;  // 数据缓冲区
    QMutex mutex;     // 保护共享数据
    qreal x_adc = 0;  // ADC 时间轴
    qreal x_cap = 0;  // CAP 时间轴
};

#endif // DATAPROCESSINGTHREAD_H
