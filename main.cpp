#include "mainwindow.h"
#include <QApplication>

#include <QDesktopWidget>

void setCenterOfApplication(QWidget* widget)
{
    QSize size = widget->sizeHint();
    QDesktopWidget* desktop = QApplication::desktop();
    int width = desktop->width();
    int height = desktop->height();
    int mw = size.width();
    int mh = size.height();
    int centerW = (width/2) ;
    int centerH = (height/2) ;
    widget->move(300, 100);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    Qt::WindowFlags flags = w.windowFlags();
    //w.setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::Tool );
    //w.setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::win );

    setCenterOfApplication(&w);

    w.show();

    return a.exec();
}
