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

QColor Dialog_Classifier_Colors::pick_color_dialog()
{
    QColor chosenColor = QColorDialog::getColor(); //return the color chosen by user

    return chosenColor;
}


void Dialog_Classifier_Colors::on_pushButton_class1_clicked()
{
    QColor qClr = pick_color_dialog();

    ui->pushButton_class1->palette();
    QPalette pal = ui->pushButton_class1->palette();
    //pal.setColor(QPalette::Button, QColor(Qt::blue));
    pal.setColor(QPalette::Button, qClr);

    ui->pushButton_class1->setAutoFillBackground(true);
    ui->pushButton_class1->setPalette(pal);
    ui->pushButton_class1->update();

}

void Dialog_Classifier_Colors::on_pushButton_class2_clicked()
{

}

void Dialog_Classifier_Colors::on_pushButton_class3_clicked()
{

}

void Dialog_Classifier_Colors::on_pushButton_class4_clicked()
{

}

void Dialog_Classifier_Colors::on_pushButton_class5_clicked()
{

}

void Dialog_Classifier_Colors::on_pushButton_class6_clicked()
{

}

void Dialog_Classifier_Colors::on_pushButton_class7_clicked()
{

}

void Dialog_Classifier_Colors::on_pushButton_class8_clicked()
{

}

void Dialog_Classifier_Colors::on_pushButton_class9_clicked()
{

}

void Dialog_Classifier_Colors::on_pushButton_class10_clicked()
{

}
