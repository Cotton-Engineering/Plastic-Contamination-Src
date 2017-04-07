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

private slots:
    void on_pushButton_clicked();

    void on_comboBox_image_processing_state_1_currentIndexChanged(const QString &arg1);

private:
    Ui::Dialog_ImageProc *ui;
};

#endif // DIALOG_IMAGEPROC_H
