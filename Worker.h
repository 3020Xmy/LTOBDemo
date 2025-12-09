#pragma once

#include <QObject>
#include <QVector>

// Worker 负责在后台线程中生成原始数据并执行降采样，
// 通过信号将进度与结果返回到 UI 线程。
class Worker : public QObject
{
    Q_OBJECT
public:
    // 可选父对象，便于 Qt 对象树管理生命周期
    explicit Worker(QObject* parent = nullptr);

public slots:
    // 生成数据并降采样到指定数量 targetCount
    void run(int targetCount);

signals:
    // 任务进度（百分比）。用于更新进度条
    void progress(int percent);
    // 生成完成后返回原始数据与降采样数据
    void resultReady(const QVector<double>& xIn,
                     const QVector<double>& yIn,
                     const QVector<double>& xOut,
                     const QVector<double>& yOut);
    // 任务结束信号，用于回收线程与恢复 UI 状态
    void finished();
};

