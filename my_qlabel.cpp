#include "my_qlabel.h"

my_qlabel::my_qlabel(QWidget *parent) : QLabel(parent)
{

}

void my_qlabel::mouseMoveEvent(QMouseEvent *event)
{
    this->x = event->x();
    this->y = event->y();

    emit Mouse_Pos();
}

void my_qlabel::mousePressEvent(QMouseEvent *event)
{

    if(event->buttons() == Qt::LeftButton)
    {
        emit Mouse_Pressed();
    }
    else if(event->buttons() == Qt::RightButton)
    {
        emit Mouse_Right_Pressed();
    }
    else
    {

    }

}

void my_qlabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if(ev->buttons() == Qt::LeftButton)
    {
        emit Mouse_Released();
    }
    else if(ev->buttons() == Qt::RightButton)
    {
        emit Mouse_Right_Released();
    }
    else
    {
        emit Mouse_Released();
    }
}

void my_qlabel::leaveEvent(QEvent *event)
{
    emit Mouse_Left();
}

