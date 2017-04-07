#ifndef DIALOG_CLASSIFIER_COLORS_H
#define DIALOG_CLASSIFIER_COLORS_H

#include <QDialog>

#include <QColorDialog>
#include <QColor>


namespace Ui {
class Dialog_Classifier_Colors;
}

class Dialog_Classifier_Colors : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Classifier_Colors(QWidget *parent = 0);
    ~Dialog_Classifier_Colors();

    QColor pick_color_dialog();

private slots:
    void on_pushButton_class1_clicked();

    void on_pushButton_class2_clicked();

    void on_pushButton_class3_clicked();

    void on_pushButton_class4_clicked();

    void on_pushButton_class5_clicked();

    void on_pushButton_class6_clicked();

    void on_pushButton_class7_clicked();

    void on_pushButton_class8_clicked();

    void on_pushButton_class9_clicked();

    void on_pushButton_class10_clicked();

private:
    Ui::Dialog_Classifier_Colors *ui;


};

#endif // DIALOG_CLASSIFIER_COLORS_H
