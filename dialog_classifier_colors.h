#ifndef DIALOG_CLASSIFIER_COLORS_H
#define DIALOG_CLASSIFIER_COLORS_H

#include <QDialog>

namespace Ui {
class Dialog_Classifier_Colors;
}

class Dialog_Classifier_Colors : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Classifier_Colors(QWidget *parent = 0);
    ~Dialog_Classifier_Colors();

private:
    Ui::Dialog_Classifier_Colors *ui;
};

#endif // DIALOG_CLASSIFIER_COLORS_H
