#include "dialog_classifier_colors.h"
#include "ui_dialog_classifier_colors.h"

Dialog_Classifier_Colors::Dialog_Classifier_Colors(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Classifier_Colors)
{
    ui->setupUi(this);
}

Dialog_Classifier_Colors::~Dialog_Classifier_Colors()
{
    delete ui;
}
