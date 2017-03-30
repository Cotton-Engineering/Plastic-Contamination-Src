#include "dialog_create_classifier.h"
#include "ui_dialog_create_classifier.h"

#include "cutils.h"


Dialog_create_classifier::Dialog_create_classifier(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_create_classifier)
{
    ui->setupUi(this);

    this->setWindowTitle("Classifier Setup and Configuration");

    //setup future watcher
    connect(&watcher,SIGNAL(finished()),this,SLOT(build_lut_finished()));


    QTimer::singleShot(250,this,SLOT(init_page()));
}

Dialog_create_classifier::~Dialog_create_classifier()
{
    delete ui;
}

void Dialog_create_classifier::init_page()
{
    ui->spinBox_class_id->setValue(cur_classId);

    if(cur_classId>=1)
    {
        ui->pushButton_clear_trainingSet->setEnabled(true);
    }
    if(cur_classId>=2)
    {
        ui->pushButton_create_compute_classifier->setEnabled(true);
    }

    int x = 0;
}


//Capture region and add to training-set
void Dialog_create_classifier::on_pushButton_capture_trainingImage_clicked()
{

    begin_create_classifier = true;

    cur_classId = ui->spinBox_class_id->value();

    if(ui->radioButton_appendRegion->isChecked())
    {
        append_to_existing_class = true;
        clear_just_one_class = false;
    }
    else
    {
        clear_just_one_class = true;
        append_to_existing_class = false;
    }

    this->close();
}


void Dialog_create_classifier::on_pushButton_load_csv_trainingSet_clicked()
{
    int x = 0;
    load_trainingSet_fromFile = true;

    cUtils cutls;

    filePathName = cutls.get_openFileName();

    int n = filePathName.length();

    if(n>0)
    {
        fileNameLoaded = true;
    }
    else
    {
        fileNameLoaded = false;
    }


    if(n>0)
    {
        bool ok = cclass->load_toCurrent_3i_trainingSet(filePathName);

        if(ok)
        {
            ui->pushButton_create_compute_classifier->setEnabled(true);
        }
        else
        {
            //open Error MsgBox
        }
    }



}

void Dialog_create_classifier::build_lut_finished()
{
    //Notify user processing is complete
    bool ok = future;

    int x = 21;

}

bool Dialog_create_classifier::run_build_lut()
{
    bool ok = cclass->build_lut();

    //restore normal cursor
    QApplication::restoreOverrideCursor();
    QApplication::processEvents();  //don't really need this here; but sometimes you do; so this is for ref

    this->close();

    return ok;
}

//build classifier
void Dialog_create_classifier::on_pushButton_create_compute_classifier_clicked()
{
    build_classifier = true;

    bool ok = cclass->build_classifier();

    if(ok)
    {
        //set wait cursor
        QApplication::setOverrideCursor(Qt::WaitCursor);


        //cclass->build_lut();

        //launch as background process
        future = QtConcurrent::run( this, &Dialog_create_classifier::run_build_lut );//note; if it needed args, just add them following func name comma separated
                                                                                     //QtConcurrent::run( this, &Dialog_create_classifier::run_build_lut, 5, ok, 2.2);

        watcher.setFuture(future);

        future.waitForFinished();  //wait here till finished processing
        //advantage of this approach is all mouse-clicks will be held for later processing
        //but you can still move windows around and kill them if you want to.


        classifier_ready_for_use = true;
    }
    else
        classifier_ready_for_use = false;

    //this->close();

}

//Final; make this classifier active, save it and exit
void Dialog_create_classifier::on_pushButton_save_classifier_exit_clicked()
{
    //get file Name to save this training Class to (saves all classes to same file for later retrieval
    cUtils cutls;

    filePathName = cutls.get_saveFileName();

    int n = filePathName.length();

    if(n>0)
    {
        fileNameLoaded = true;
    }
    else
    {
        fileNameLoaded = false;
    }

    save_to_file = true;

    save_current_classifier = true;
    this->close();
}

//save current training set to file
void Dialog_create_classifier::on_pushButton_save_trainingSet_clicked()
{
    //get file Name to save this training Class to (saves all classes to same file for later retrieval
    cUtils cutls;

    filePathName = cutls.get_saveFileName();
    //save_to_file = false;

    int n = filePathName.length();

    if(n>0)
    {
        fileNameLoaded = true;
        save_current_trainingSet = true;
    }
    else
    {
        fileNameLoaded = false;
        save_current_trainingSet = false;
    }

    //check if append mode
    if(ui->radioButton_appendRegion)
    {
        append_to_existing_trainingSet = true;
    }
    else
    {
        append_to_existing_trainingSet = false;
    }

    save_current_classifier = false;

    bool ok = cclass->save_current_3i_trainingSet(filePathName, QString("Label_Id,B,G,R"), append_to_existing_trainingSet);

    if(!ok)
    {
        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Error Saving Current Training Set (3i)");
        return;
    }
    //save_current_trainingSet = false;
    //this->close();
}

void Dialog_create_classifier::on_pushButton_save_classifier_clicked()
{

}

//This option assigns all pixels in this image to this class (no region capture required)
void Dialog_create_classifier::on_pushButton_use_full_image_for_training_this_class_clicked()
{

}

void Dialog_create_classifier::on_pushButton_clear_trainingSet_clicked()
{
    cclass->clear_classifier();

    cur_classId = 0;

    ui->spinBox_class_id->setValue(cur_classId);


}

void Dialog_create_classifier::on_pushButton_load_classifier_clicked()
{
    plot3d_python();
}

void Dialog_create_classifier::plot3d_python()
{
    QProcess p;
    QStringList params;

    //params << "python_test_plot3d.py -arg1 arg1";
    params << "/home/cfd0/Documents/python_test_plot3d.py";

    p.start("python", params);
    p.waitForFinished(-1);

    QString p_stdout = p.readAll();

    ui->lineEdit->setText(p_stdout);


}

