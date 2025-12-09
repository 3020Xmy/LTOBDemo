#include "LTOBDemo.h"
#include <QtWidgets/QApplication>

// 程序入口：创建 Qt 应用并显示主窗口。

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LTOBDemo w;
    w.show();
    return a.exec();
}
