#pragma once

#include <QtWidgets/QWidget>
#include "ui_LTOBDemo.h"
#include <QThread>
class Worker;

QT_BEGIN_NAMESPACE
namespace Ui { class LTOBDemoClass; };
QT_END_NAMESPACE

// 主窗口：负责初始化图表、启动后台线程生成数据并更新 UI。
class LTOBDemo : public QWidget
{
    Q_OBJECT

public:
    LTOBDemo(QWidget *parent = nullptr);
    ~LTOBDemo();

private:
    Ui::LTOBDemoClass *ui;         // 由 Qt Designer 生成的 UI 封装
    QThread* workerThread = nullptr; // 工作线程，运行数据生成逻辑
    bool busy = false;             // 防止重复触发生成

private slots:
    // 点击生成按钮时启动后台任务
    void onGenerateClicked();
    // 更新进度条
    void onProgress(int percent);
    // 接收原始与降采样数据并更新两张图
    void onResultReady(const QVector<double>& xIn,
                       const QVector<double>& yIn,
                       const QVector<double>& xOut,
                       const QVector<double>& yOut);
    // 任务结束后恢复按钮与状态
    void onWorkerFinished();
};
