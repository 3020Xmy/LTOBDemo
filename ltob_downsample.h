#pragma once

#include <QVector>

// 使用 LTOB（Largest Triangle One Bucket）算法对二维数据进行降采样。
// 该方法在每个桶中选择与相邻均值点组成的三角形面积最大的点，
// 在尽量保留形状特征的同时显著降低点数。
void ltobDownsample(const QVector<double>& xIn,
                    const QVector<double>& yIn,
                    QVector<double>& xOut,
                    QVector<double>& yOut,
                    int targetCount);

