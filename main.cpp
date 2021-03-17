#include "widget.h"
#include"dialoglist.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Widget w;
//    w.show();

    DialogList list;
    list.show();  //显示主场景窗口

    return a.exec();
}
