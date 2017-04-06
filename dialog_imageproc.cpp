#include "dialog_imageproc.h"
#include "ui_dialog_imageproc.h"

Dialog_ImageProc::Dialog_ImageProc(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_ImageProc)
{
    ui->setupUi(this);
}

Dialog_ImageProc::~Dialog_ImageProc()
{
    delete ui;
}
