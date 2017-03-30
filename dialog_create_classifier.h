#ifndef DIALOG_CREATE_CLASSIFIER_H
#define DIALOG_CREATE_CLASSIFIER_H

#include <QDialog>
#include <QTimer>
#include <QString>
#include <QStringList>

#include <QProcess>
//#include <QThread>

//for calling a function as a background process
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>


#include "cclassifier.h"


namespace Ui {
class Dialog_create_classifier;
}

class Dialog_create_classifier : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_create_classifier(QWidget *parent = 0);
    ~Dialog_create_classifier();

private slots:

    void build_lut_finished();

    void init_page();

    //Capture region and add to training-set
    void on_pushButton_capture_trainingImage_clicked();

    void on_pushButton_load_csv_trainingSet_clicked();

    void on_pushButton_create_compute_classifier_clicked();

    void on_pushButton_save_classifier_exit_clicked();

    void on_pushButton_save_trainingSet_clicked();

    void on_pushButton_save_classifier_clicked();

    void on_pushButton_use_full_image_for_training_this_class_clicked();

    void on_pushButton_clear_trainingSet_clicked();

    void on_pushButton_load_classifier_clicked();

public:
    void plot3d_python();

    bool begin_create_classifier = false;  //add a new region to trainingSet
    bool build_classifier = false;  //build classifier with currently defined trainingSet
    bool clear_classifier = false;  //clear all regions from trainingSet
    bool clear_just_one_class = false;
    bool run_histogram = false;

    bool save_current_trainingSet = false;
    bool save_current_classifier = false;
    bool load_trainingSet_fromFile = false;

    bool classifier_ready_for_use = false;

    bool save_to_file = true;
    bool fileNameLoaded = false;
    QString filePathName;

    bool append_to_existing_class = true;
    bool append_to_existing_trainingSet = true;
    QString sHeaderLine = "Class_Id,Blue,Green,Red";

    int cur_classId;

    cClassifier *cclass;

private:
    Ui::Dialog_create_classifier *ui;

    QFuture<bool> future;
    QFutureWatcher<bool> watcher;

    bool run_build_lut();



};

#endif // DIALOG_CREATE_CLASSIFIER_H
