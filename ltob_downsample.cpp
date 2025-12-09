#include "ltob_downsample.h"
#include <cmath>

// LTOB（Largest Triangle One Bucket）降采样实现。
// 思路：
// - 保留首尾点
// - 将剩余点均匀划分为若干桶
// - 对每个桶，从上一个已选点出发，选择与后续均值点构成三角形面积最大的点
// 这样可以在较少点的情况下保留曲线轮廓的关键变化。

/*
*   @brief 计算三角形面积
*   @param[in] ax：顶点A的X轴坐标
*   @param[in] ay：顶点A的Y轴坐标
*   @param[in] bx：顶点B的X轴坐标
*   @param[in] by：顶点B的Y轴坐标
*   @param[in] cx：顶点C的X轴坐标
*   @param[in] cy：顶点C的Y轴坐标
*/
static inline double triArea(double ax, double ay, double bx, double by, double cx, double cy)
{
    return std::abs((ax - cx) * (by - ay) - (ax - bx) * (cy - ay));
}

/*
*   @brief LTOB时间序列降采样算法
*   @param[in] xIn：输入的X轴数据
*   @param[in] yIn：输入的Y轴数据
*   @param[out] xOut：输出的X轴数据
*   @param[out] yOut：输出的Y轴数据
*   @param[in] targetCount：目标点数
*/
void ltobDownsample(const QVector<double>& xIn, const QVector<double>& yIn,
                    QVector<double>& xOut, QVector<double>& yOut, int targetCount)
{
    xOut.clear();
    yOut.clear();

    const int n = xIn.size();   // 输入数据大小
    if (n == 0 || targetCount <= 0) {   // 空数据或目标数量非法直接返回
        return;
    }
    if (targetCount >= n) {         // 不需要降采样：直接返回原始数据
        xOut = xIn;
        yOut = yIn;
        return;
    }
    if (targetCount == 1) {        // 只取首个点
        xOut.push_back(xIn.front());
        yOut.push_back(yIn.front());
        return;
    }

    xOut.reserve(targetCount);
    yOut.reserve(targetCount);

    // 保留第一个点
    xOut.push_back(xIn[0]);
    yOut.push_back(yIn[0]);

    int a = 0; // 上一个已选点的索引
    const double bucketSize = double(n - 2) / double(targetCount - 2);
    for (int i = 0; i < targetCount - 2; ++i) {
        const int bucketStart = int(std::floor(1 + i * bucketSize));
        const int bucketEnd = int(std::floor(1 + (i + 1) * bucketSize));

        // 计算下一桶的均值点（用于与候选点构成三角形）
        double avgX = 0.0, avgY = 0.0;
        int avgStart = int(std::floor(1 + (i + 1) * bucketSize));
        int avgEnd = int(std::floor(1 + (i + 2) * bucketSize));
        if (avgEnd > n - 1) {
            avgEnd = n - 1;
        }
        int avgCount = std::max(1, avgEnd - avgStart);
        for (int k = avgStart; k < avgEnd; ++k) {
            avgX += xIn[k];
            avgY += yIn[k];
        }
        avgX /= avgCount;
        avgY /= avgCount;

        // 在当前桶中选择面积最大的候选点
        double maxArea = -1.0;
        int chosen = bucketStart;
        int bEnd = std::max(bucketStart + 1, bucketEnd);
        for (int j = bucketStart; j < bEnd; ++j) {
            double area = triArea(xIn[a], yIn[a], xIn[j], yIn[j], avgX, avgY);
            if (area > maxArea) {
                maxArea = area;
                chosen = j;
            }
        }
        xOut.push_back(xIn[chosen]);
        yOut.push_back(yIn[chosen]);
        a = chosen;
    }

    // 保留最后一个点
    xOut.push_back(xIn[n - 1]);
    yOut.push_back(yIn[n - 1]);
}

