#ifndef DIALOG_IMAGEPROC_H
#define DIALOG_IMAGEPROC_H

#include <QDialog>

namespace Ui {
class Dialog_ImageProc;
}

class Dialog_ImageProc : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_ImageProc(QWidget *parent = 0);
    ~Dialog_ImageProc();

private:
    Ui::Dialog_ImageProc *ui;
};

#endif // DIALOG_IMAGEPROC_H
