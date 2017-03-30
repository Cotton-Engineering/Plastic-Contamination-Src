#ifndef MY_QLABEL_H
#define MY_QLABEL_H

#include <QWidget>
#include <QMouseEvent>
#include <QDebug>
#include <QEvent>
#include <QLabel>

//good video how-to on subclassing to get mouse events
// https://www.youtube.com/watch?v=d0CDMtfefB4

class my_qlabel : public QLabel
{
    Q_OBJECT
public:
    explicit my_qlabel(QWidget *parent = 0);

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *ev);

    void leaveEvent(QEvent *event);

    int x,y;

signals:
    void Mouse_Pressed();
    void Mouse_Right_Pressed();
    void Mouse_Pos();
    void Mouse_Left();
    void Mouse_Released();
    void Mouse_Right_Released();

public slots:
};

#endif // MY_QLABEL_H
