#include "LTOBDemo.h"
#include "Worker.h"
#include "ltob_downsample.h"
#include <QPen>
#include <QTimer>
#include "qCustomPlot/qcustomplot.h"

// 主窗口实现：负责配置图表、接线信号与槽以及展示结果。
LTOBDemo::LTOBDemo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LTOBDemoClass())
{
    ui->setupUi(this);

    // 配置“原始数据”图表样式并添加标题
    ui->plotOriginal->addGraph();
    ui->plotOriginal->graph(0)->setPen(QPen(QColor(0, 180, 0), 2));
    ui->plotOriginal->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);
    ui->plotOriginal->addGraph();
    ui->plotOriginal->graph(1)->setPen(QPen(QColor(200, 0, 0), 2));
    ui->plotOriginal->graph(1)->setScatterStyle(QCPScatterStyle::ssNone);
    ui->plotOriginal->graph(0)->setName("原始数据");
    ui->plotOriginal->graph(1)->setName("降采样数据");
    ui->plotOriginal->xAxis->grid()->setVisible(true);
    ui->plotOriginal->yAxis->grid()->setVisible(true);
    ui->plotOriginal->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->plotOriginal->axisRect()->setRangeDragAxes(ui->plotOriginal->xAxis, ui->plotOriginal->yAxis);
    ui->plotOriginal->axisRect()->setRangeZoomAxes(ui->plotOriginal->xAxis, ui->plotOriginal->yAxis);
    ui->plotOriginal->plotLayout()->insertRow(0);
    ui->plotOriginal->plotLayout()->addElement(0, 0, new QCPTextElement(ui->plotOriginal, "原始数据与降采样数据"));
    ui->plotOriginal->legend->setVisible(true);
    ui->plotOriginal->axisRect()->insetLayout()->addElement(ui->plotOriginal->legend, Qt::AlignTop | Qt::AlignRight);

    // 绑定按钮点击以启动后台生成任务
    connect(ui->btnGenerate, &QPushButton::clicked, this, &LTOBDemo::onGenerateClicked);
}

LTOBDemo::~LTOBDemo()
{
    if (workerThread) {
        // 优雅退出线程并回收资源
        workerThread->quit();
        workerThread->wait();
        workerThread->deleteLater();
        workerThread = nullptr;
    }
    delete ui;
}

void LTOBDemo::onGenerateClicked()
{
    if (busy) return;
    busy = true;
    ui->btnGenerate->setEnabled(false);
    ui->progressBar->setValue(0);

    int targetCount = ui->spinTargetCount->value();

    // 创建 Worker 并将其移动到新线程
    Worker* worker = new Worker();
    workerThread = new QThread(this);
    worker->moveToThread(workerThread);

    // 在线程启动时调用 Worker::run，连接信号用于更新 UI 与结束处理
    connect(workerThread, &QThread::started, [worker, targetCount]() {
        worker->run(targetCount);
    });
    connect(worker, &Worker::progress, this, &LTOBDemo::onProgress);
    connect(worker, &Worker::resultReady, this, &LTOBDemo::onResultReady);
    connect(worker, &Worker::finished, this, &LTOBDemo::onWorkerFinished);
    connect(worker, &Worker::finished, workerThread, &QThread::quit);
    connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);

    workerThread->start();
}

void LTOBDemo::onProgress(int percent)
{
    ui->progressBar->setValue(percent);
}

void LTOBDemo::onResultReady(const QVector<double>& xIn,
                             const QVector<double>& yIn,
                             const QVector<double>& xOut,
                             const QVector<double>& yOut)
{
    // 更新原始数据曲线
    ui->plotOriginal->graph(0)->setData(xIn, yIn);
    ui->plotOriginal->graph(1)->setData(xOut, yOut);
    ui->plotOriginal->rescaleAxes();
    ui->plotOriginal->replot();
}

void LTOBDemo::onWorkerFinished()
{
    // 恢复按钮与忙状态标记
    ui->btnGenerate->setEnabled(true);
    busy = false;
}
