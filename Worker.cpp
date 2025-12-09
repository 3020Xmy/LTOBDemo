#include "Worker.h"
#include "ltob_downsample.h"
#include <QRandomGenerator>

// 后台数据生成与降采样的实现。
// 每次运行都会随机化趋势、幅值、频率、相位与噪声强度，确保数据不同。

// 构造函数，保持默认行为
Worker::Worker(QObject* parent) : QObject(parent) {}

void Worker::run(int targetCount)
{
    // 原始数据点数量（较大以体现降采样效果）
    const int N = 10000;
    QVector<double> xIn;
    QVector<double> yIn;
    xIn.resize(N);
    yIn.resize(N);

    // 随机化数据参数，使每次生成的数据形态不同
    auto rng = QRandomGenerator::global();
    const double slope = 0.0002 + rng->generateDouble() * 0.0018;    // 线性趋势斜率
    const double amp   = 5.0    + rng->generateDouble() * 15.0;      // 正弦信号幅值
    const double freq  = 0.005  + rng->generateDouble() * 0.020;     // 正弦信号频率
    const double phase = rng->generateDouble() * 6.283185307179586;  // 相位（0~2π）
    const double noiseScale = 0.5 + rng->generateDouble() * 2.0;     // 噪声强度系数

    for (int i = 0; i < N; ++i) {
        xIn[i] = i;
        double trend = slope * i;                                     // 线性趋势
        double signal = std::sin(i * freq + phase) * amp;             // 正弦成分
        double noise = noiseScale * (rng->bounded(-100, 100) / 100.0);// 随机噪声
        yIn[i] = trend + signal + noise;
        // 间隔触发进度更新（上限 50%），剩余进度在计算完成后补齐
        if (i % (N / 10) == 0) emit progress(i * 50 / N);
    }

    QVector<double> xOut;
    QVector<double> yOut;
    // 使用 LTOB 算法将数据降采样到 targetCount
    ltobDownsample(xIn, yIn, xOut, yOut, targetCount);

    emit progress(100);
    emit resultReady(xIn, yIn, xOut, yOut);
    emit finished();
}

