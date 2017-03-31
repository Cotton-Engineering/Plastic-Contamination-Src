#define Includes_Start {

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QSettings>

#include <QMessageBox>
#include <QDesktopServices>
#include <QStandardPaths>

#include <QLabel>
#include <QPainter>

#include "my_qlabel.h"

#define Includes_End }



#define Constructor_Destructor_Start {
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("USDA-ARS Plastic Contamination Detection System");

    cocv = new cOpenCv_Utils();
    img_mouse_captured = false;
    mouse_capture_count=0;

    cclass = new cClassifier();

    cObjClasser = new cObjectClassifier();



    loopTimerStarted = false;

    sInit_fileName = ":/ini";  //path name to Resource located file 'ini' in config.qrc

    bProcessPixels=false;
    bObjectProcess=false;

    bUseHSV = true;
    bLiveVideo = true;
    bProcessImage = false;

    bClearBackground = false;
    bTuneYellow = false;
    bTuneBlue = false;

    bTuneRed = false;
    bTuneRed2 = false;
    bTuneGreen = false;
    bTuneCyan = false;

    bTuneS = false;
    bTuneV = false;

    bFltr_CLAHE = false;

    //default settings
    low_satVal = 66;
    high_satVal = 248;
    low_blue_satVal = 66;
    high_blue_satVal = 248;
    low_grn_satVal = 66;
    high_grn_satVal = 248;
    low_red_satVal = 66;
    high_red_satVal = 248;
    low_red2_satVal = 66;
    high_red2_satVal = 248;
    low_yellow_satVal = 66;
    high_yellow_satVal = 248;
    low_cyan_satVal = 66;
    high_cyan_satVal = 248;
    low_val = 0;
    high_val = 255;

    low_blue_h = 100;
    high_blue_h = 117;
    low_grn_h = 45;
    high_grn_h = 75;
    low_red_h = 150;
    high_red_h = 162;
    low_red2_h = 0;
    high_red2_h = 5;
    low_yellow_h = 20;
    high_yellow_h = 50;
    low_cyan_h = 80;
    high_cyan_h = 100;

    //low_blue_h = 115;


    QString m_path ;
    QString m_filename;
    //QSettings * p_settings;

    m_path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) ;
    m_filename = "config.ini" ;

    p_settings = new QSettings(m_path + "/"+ m_filename,QSettings::IniFormat);


    //saveSettings();

    //load from resource file if possible (if not fall back to already loaded values
    loadSettings();  //load up settings from ini file in config.qrc

    int lbh = low_blue_h;
    int cbh = high_cyan_h;
    int cbl = low_cyan_h;


    skipCount = 0;
    first_time = true;

    serial = new QSerialPort(this);


    //connect(serial, &QSerialPort::readyRead, this, &MainWindow::serialRead);
    connect(serial,SIGNAL(readyRead()),this,SLOT(serialRead()));

    initSerialPort();

    //"Display window"
    //cv::namedWindow("Display_Image",WINDOW_NORMAL);
    //cv::resizeWindow("Display_Image", 800,600);

    //need to shift focus back on us
    //this->activateWindow();

    //setup mouse slots
    connect(ui->label_imageShow,SIGNAL(Mouse_Pos()),this,SLOT(Mouse_current_pos()));
    connect(ui->label_imageShow,SIGNAL(Mouse_Pressed()),this,SLOT(Mouse_Pressed()));
    connect(ui->label_imageShow,SIGNAL(Mouse_Right_Pressed()),this,SLOT(Mouse_Right_Pressed()));
    connect(ui->label_imageShow,SIGNAL(Mouse_Left()),this,SLOT(Mouse_left()));
    connect(ui->label_imageShow,SIGNAL(Mouse_Released()),this,SLOT(Mouse_Released()));



    //load initial image
    load_image();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//key capture for image zoom and pan
void MainWindow::keyPressEvent( QKeyEvent* event )
{
    if(!event->isAutoRepeat())  //only adjust on 1st press event; ignore
    {
        switch ( event->key() )
        {
            case Qt::Key_Plus:
                //act on '+' (zoom In)
                zoom_factor++;
                break;
            case Qt::Key_Minus:
                //act on '-' (zoom Out)
                zoom_factor--;
                break;
            case Qt::Key_Escape:
                //act on 'Escape' (reset mouse capture classifier region)
                init_capture();  //needed so we can reset between zoom in and out
                break;
            default:
                event->ignore();
                break;
        }

        if(zoom_factor>1)
        {
            isZoomed = true;
        }
        else
        {
            zoom_factor = 1;
            isZoomed = false;
        }



    }
    else
    {
        switch ( event->key() )
        {
            case Qt::Key_4:
                //act on '4' (pan Left)
                pan_topLeft.x++;
                break;
            case Qt::Key_6:
                //act on '6' (pan Right)
                pan_topLeft.x--;
                break;
            case Qt::Key_8:
                //act on '8' (pan Up)
                pan_topLeft.y++;
                break;
            case Qt::Key_2:
                //act on '2' (pan Down)
                pan_topLeft.y--;
                break;
            default:
                event->ignore();
                break;
        }


        if(pan_topLeft.x < 0)
        {
            pan_topLeft.x = 0;
        }
        if(pan_topLeft.y < 0)
        {
            pan_topLeft.y = 0;
        }
    }

}

void MainWindow::Mouse_current_pos()
{
    int x = ui->label_imageShow->x;
    int y = ui->label_imageShow->y;

    img_mouse_pos.x = x;
    img_mouse_pos.y = y;
}

void MainWindow::Mouse_Pressed()
{
    if(mouse_firstPress)
    {
        img_mouse_captured = true;
        img_mouse_lastCaptured = img_mouse_pos;
        mouse_firstPress = false;
    }
    else
    {
        img_mouse_captured = false;
    }
}
void MainWindow::Mouse_Right_Pressed()
{
    if(mouse_right_firstPress)
    {
        img_mouse_captured = true;
        mouse_right_firstPress = false;
        img_mouse_right_captured = true;
    }
    else
    {
        img_mouse_right_captured = false;
    }
}

void MainWindow::Mouse_Released()
{
    mouse_firstPress = true;
}

void MainWindow::Mouse_Right_Released()
{
    mouse_right_firstPress = true;
}


void MainWindow::Mouse_left()
{

}

void MainWindow::clear_mouse_capture_buffer()
{
    mouse_capture_count = -1;
    for(int i=0;i<MOUSE_CAPTURE_BUFFER_SIZE;i++)
    {
        img_mousePts_captured[i].x = 0;
        img_mousePts_captured[i].y = 0;

    }

}

void MainWindow::init_capture()
{
    clear_mouse_capture_buffer();
    mouse_capture_count=-1;
    rubber_band_on = true;
    mouse_capture_complete = false;

    img_mouse_right_captured = false;

    mouse_right_firstPress = true;  //reset right-click close polygon mode

    img_mouse_captured = false; //start with mouse capture off

}

#define Constructor_Destructor_End }



#define Menu_Action_Triggers_Start {
/// ***************************************************
/// Menu Action Triggers


void MainWindow::on_action_Start_triggered()
{
    /// *******************************************
    ///   IMPORTANT; USE USB 3.0 INPUT EVEN FOR USB 2.0 CAMERAS!
    ///              otherwise you'll get bad frames periodically
    /// *******************************************

    //open USB Camera
    if( !mCapture.isOpened() )
        if( !mCapture.open( 0 ) )
            return;

    //set resolution here
    //cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
    //cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);


    bLiveVideo = true;
    ui->action_Live_Video->setChecked(true);

    bProcessImage = false;
    ui->action_Process_Image_as_Live->setChecked(false);

    if(!loopTimerStarted)
    {
        loopTimerStarted = true;
        startTimer(10);  //set for 50ms timer rate
    }
}

void MainWindow::on_actionImage_Save_triggered()
{
    //check if we have a valid image (i.e. camera is capturing
    if( !mCapture.isOpened() )
    {
        QMessageBox::information(this, tr("Error"),tr("Unable to save Image File; Camera needs to be Started"));
        return;
    }

    //check if live video is off
    if (bLiveVideo)
    {
        QMessageBox::information(this, tr("Error"),tr("Unable to save Image File while Video is running"));
        return;
    }



    QString homeLocation = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
    QString pics_path = QStandardPaths::locate(QStandardPaths::PicturesLocation, QString(), QStandardPaths::LocateDirectory);

    QFileDialog qfd;

    qfd.setDirectory(pics_path);

    //get save path
    QString filePathName = qfd.getSaveFileName(this, tr("Save Image"), "", tr("Image (*.png);Image (*.jpg);All Files (*)"));


    if (filePathName.isEmpty())
        return;
    else
    {
        //need to test ext to make sure it's png or jpg (valid image extension
        QList<QString> ext_list;
        ext_list<<"bmp"<<"jpg"<<"png";
        QFileInfo fi(filePathName);
        QString ext = fi.suffix();
        if (ext_list.contains(ext))
        {
            //will write pic type based on provided extension?
            write_cvImage(image_copy, filePathName);
        }
        else
        {
            QMessageBox msgBox;
            msgBox.critical(0, "Error", "You must select a valid image file (ensure extension is {*.png, *.jpg, *.bmp}");
            return;
        }

    }

}

void MainWindow::on_action_Open_Image_triggered()
{

    load_image();

/*
    bLiveVideo = false;
    ui->action_Live_Video->setChecked(false);

    QString pics_path = QStandardPaths::locate(QStandardPaths::PicturesLocation, QString(), QStandardPaths::LocateDirectory);

    QFileDialog qfd;

    qfd.setDirectory(pics_path);

    //get save path
    QString filePathName = qfd.getOpenFileName(this,tr("Open Image"),"",tr("Image (*.png);Image (*.jpg);All Files (*)"));
    //QString filePathName = qfd.getSaveFileName(this, tr("Save Image"), "", tr("Image (*.png);Image (*.jpg);All Files (*)"));


    if (filePathName.isEmpty())
        return;
    else
    {
        //need to test ext to make sure it's png or jpg (valid image extension
        QList<QString> ext_list;
        ext_list<<"bmp"<<"jpg"<<"png";
        QFileInfo fi(filePathName);
        QString ext = fi.suffix();
        if (ext_list.contains(ext))
        {
            read_cvImage(image_copy, filePathName);  //so, we're going to read this into image_copy; how to let process know to use it?

            bProcessImage = true;
            //ui->action_Open_Image->setChecked(true);
            ui->action_Process_Image_as_Live->setChecked(true);

            ui->action_Process_Image_as_Live->setEnabled(true);

            if(!loopTimerStarted)
            {
                loopTimerStarted = true;
                startTimer(10); //want to be able to treat image as live-video; question will be how to switch to live video from still image...?
            }
        }
        else
        {
            QMessageBox msgBox;
            msgBox.critical(0, "Error", "You must select a valid image file (ensure extension is {*.png, *.jpg, *.bmp}");
            return;
        }

    }

*/
}

void MainWindow::on_action_Process_Image_as_Live_triggered(bool checked)
{
    bProcessImage = checked;
}

void MainWindow::on_action_Process_Image_triggered(bool checked)
{
    bProcessPixels = checked;
}

void MainWindow::on_action_Object_Process_triggered(bool checked)
{
    bObjectProcess = checked;
}

void MainWindow::on_actionUse_HSV_triggered(bool checked)
{
    bUseHSV = checked;
}


void MainWindow::on_actionUse_Bayes_Classifier_triggered(bool checked)
{
    bUseBayes = checked;
}

void MainWindow::on_action_Live_Video_triggered(bool checked)
{
    bLiveVideo = checked;
}

void MainWindow::on_action_Clear_Background_triggered(bool checked)
{
    bClearBackground = checked;
}

void MainWindow::on_actionTune_Hue_Blue_triggered(bool checked)
{
    turn_off_all_HUV_tuners();

    bTuneBlue = checked;
    ui->actionTune_Hue_Blue->setChecked(checked);
}

void MainWindow::on_actionTune_Hue_Yellow_triggered(bool checked)
{
    turn_off_all_HUV_tuners();

    bTuneYellow = checked;
    ui->actionTune_Hue_Yellow->setChecked(checked);
}

void MainWindow::on_actionTune_Hue_Green_triggered(bool checked)
{
    turn_off_all_HUV_tuners();

    bTuneGreen = checked;
    ui->actionTune_Hue_Green->setChecked(checked);
}

void MainWindow::on_actionTune_Hue_Red_triggered(bool checked)
{
    turn_off_all_HUV_tuners();

    bTuneRed = checked;
    ui->actionTune_Hue_Red->setChecked(checked);
}

void MainWindow::on_actionTune_Hue_Red2_triggered(bool checked)
{
    turn_off_all_HUV_tuners();

    bTuneRed2 = checked;
    ui->actionTune_Hue_Red2->setChecked(checked);
}

void MainWindow::on_actionTune_Hue_Cyan_triggered(bool checked)
{
    turn_off_all_HUV_tuners();

    bTuneCyan = checked;
    ui->actionTune_Hue_Cyan->setChecked(checked);
}

void MainWindow::on_actionTune_Hue_Saturation_triggered(bool checked)
{
    bTuneS = checked;

    if(bTuneS)
    {
        bTuneV = false;
        ui->actionTune_Hue_Value->setChecked(false);
    }
}

void MainWindow::on_actionTune_Hue_Value_triggered(bool checked)
{
    bTuneV = checked;

    if(bTuneV)
    {
        bTuneS = false;
        ui->actionTune_Hue_Saturation->setChecked(false);
    }
}

void MainWindow::turn_off_all_HUV_tuners()
{
    bTuneYellow = false;
    bTuneBlue = false;

    bTuneRed = false;
    bTuneRed2 = false;
    bTuneGreen = false;
    bTuneCyan = false;

    ui->actionTune_Hue_Blue->setChecked(false);
    ui->actionTune_Hue_Cyan->setChecked(false);
    ui->actionTune_Hue_Green->setChecked(false);
    ui->actionTune_Hue_Red->setChecked(false);
    ui->actionTune_Hue_Red2->setChecked(false);
    ui->actionTune_Hue_Yellow->setChecked(false);

}


void MainWindow::on_actionSave_Pixel_Class_Values_triggered()
{
    saveSettings();
    //QMessageBox
}



void MainWindow::on_actionCLAHE_Filter_triggered(bool checked)
{
    bFltr_CLAHE = checked;
}




void MainWindow::on_actionCreate_Classifier_triggered()
{
    if(loopTimerStarted)
    {
        dcc = new Dialog_create_classifier();

        dcc->cur_classId = cur_classId;  //let it know where we're currently at
        dcc->cclass = cclass;  //pass ptr to our classifier; so it can call it directly

        //dcc->setModal(true);
        //dcc->setWindowModality(Qt::WindowModal);

        //dcc->show(); //this will open dialog and continue execution; thereby running in parallel
        dcc->exec();  //this will open dialog and pause execution here till it returns; note; only avail for QDialog...
        //dcc->open(); //not sure what difference this is from show()...

        run_histogram = dcc->run_histogram;

        cur_classId = dcc->cur_classId;  //save what cur classId was set to

        classifier_ready_for_use = dcc->classifier_ready_for_use;  //see if we've built classifier and it's ready to use

        init_capture();
        /*  //move to init_capture()
            clear_mouse_capture_buffer();
            mouse_capture_count=-1;
            rubber_band_on = true;
            mouse_capture_complete = false;

            img_mouse_right_captured = false;

            mouse_right_firstPress = true;  //reset right-click close polygon mode

            img_mouse_captured = false; //start with mouse capture off
        */


        createClassifier_start = dcc->begin_create_classifier;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Error: Must be running on picture or live video; please start one of these processes... ");

        return;
    }




}

#define Menu_Action_Triggers_End }

#define Config_Setup_Start {

void MainWindow::loadSettings()
{

    //QSettings settings(sInit_fileName, QSettings::IniFormat);

    //check that we can read it (that it's not missing)
    p_settings->beginGroup("GROUP_TEST");
    int param1 = p_settings->value("PARAM1").toInt();
    p_settings->endGroup();

    if(param1 == 99)  //should read 99 if it's there and a correct file
    {
        p_settings->beginGroup("GROUP_SaturValue");
        //QString param3b = settings.value("PARAM3").toString();  //example on how to read in string
        low_satVal = p_settings->value("low_satVal").toInt();
        high_satVal = p_settings->value("high_satVal").toInt();
        low_val = p_settings->value("low_val").toInt();
        high_val = p_settings->value("high_val").toInt();
        low_blue_satVal = p_settings->value("low_blue_satVal").toInt();
        high_blue_satVal = p_settings->value("high_blue_satVal").toInt();
        low_grn_satVal = p_settings->value("low_grn_satVal").toInt();
        high_grn_satVal = p_settings->value("high_grn_satVal").toInt();
        low_red_satVal = p_settings->value("low_red_satVal").toInt();
        high_red_satVal = p_settings->value("high_red_satVal").toInt();
        low_red2_satVal = p_settings->value("low_red2_satVal").toInt();
        high_red2_satVal = p_settings->value("high_red2_satVal").toInt();
        low_yellow_satVal = p_settings->value("low_yellow_satVal").toInt();
        high_yellow_satVal = p_settings->value("high_yellow_satVal").toInt();
        low_cyan_satVal = p_settings->value("low_cyan_satVal").toInt();
        high_cyan_satVal = p_settings->value("high_cyan_satVal").toInt();
        p_settings->endGroup();

        p_settings->beginGroup("GROUP_HUES");
        low_blue_h = p_settings->value("low_blue_h").toInt();
        high_blue_h = p_settings->value("high_blue_h").toInt();
        low_grn_h = p_settings->value("low_grn_h").toInt();
        high_grn_h = p_settings->value("high_grn_h").toInt();
        low_red_h = p_settings->value("low_red_h").toInt();
        high_red_h = p_settings->value("high_red_h").toInt();
        low_red2_h = p_settings->value("low_red2_h").toInt();
        high_red2_h = p_settings->value("high_red2_h").toInt();
        low_yellow_h = p_settings->value("low_yellow_h").toInt();
        high_yellow_h = p_settings->value("high_yellow_h").toInt();
        low_cyan_h = p_settings->value("low_cyan_h").toInt();
        high_cyan_h = p_settings->value("high_cyan_h").toInt();
        p_settings->endGroup();

    }


}

void MainWindow::saveSettings()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Save Configuration", "Are you sure you want to Save Pixel Classifier Values?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        //qDebug() << "Yes was clicked";
        //QApplication::quit();

        //QSettings settings(sInit_fileName, QSettings::IniFormat);

        p_settings->beginGroup("GROUP_TEST");
        p_settings->setValue("PARAM1", 99);
        p_settings->endGroup();

        p_settings->beginGroup("GROUP_SaturValue");
        p_settings->setValue("low_satVal", low_satVal);
        p_settings->setValue("high_satVal", high_satVal);
        p_settings->setValue("low_val", low_val);
        p_settings->setValue("high_val", high_val);
        p_settings->setValue("low_blue_satVal", low_blue_satVal);
        p_settings->setValue("high_blue_satVal", high_blue_satVal);
        p_settings->setValue("low_grn_satVal", low_grn_satVal);
        p_settings->setValue("high_grn_satVal", high_grn_satVal);
        p_settings->setValue("low_red_satVal", low_red_satVal);
        p_settings->setValue("high_red_satVal", high_red_satVal);
        p_settings->setValue("low_red2_satVal", low_red2_satVal);
        p_settings->setValue("high_red2_satVal", high_red2_satVal);
        p_settings->setValue("low_yellow_satVal", low_yellow_satVal);
        p_settings->setValue("high_yellow_satVal", high_yellow_satVal);
        p_settings->setValue("low_cyan_satVal", low_cyan_satVal);
        p_settings->setValue("high_cyan_satVal", high_cyan_satVal);
        p_settings->endGroup();

int lbh = low_blue_h;
int hbh = high_blue_h;



        p_settings->beginGroup("GROUP_HUES");
        p_settings->setValue("low_blue_h", low_blue_h);
        p_settings->setValue("high_blue_h", high_blue_h);
        p_settings->setValue("low_grn_h", low_grn_h);
        p_settings->setValue("high_grn_h", high_grn_h);
        p_settings->setValue("low_red_h", low_red_h);
        p_settings->setValue("high_red_h", high_red_h);
        p_settings->setValue("low_red2_h", low_red2_h);
        p_settings->setValue("high_red2_h", high_red2_h);
        p_settings->setValue("low_yellow_h", low_yellow_h);
        p_settings->setValue("high_yellow_h", high_yellow_h);
        p_settings->setValue("low_cyan_h", low_cyan_h);
        p_settings->setValue("high_cyan_h", high_cyan_h);
        p_settings->endGroup();

    }
    else
    {
        //qDebug() << "No was clicked";
        return;
    }




}

#define Config_Setup_End }

#define Serial_Port_Start {

void MainWindow::initSerialPort()
{
    QString er_msg;


    //connect(serial,SIGNAL(readyRead()),this,SLOT(serialRead()));
    //connect(serial,SIGNAL(readyRead()),this,SLOT(serialRead()));

    //connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);


    //get name for choose
    QList<QSerialPortInfo>  infos = QSerialPortInfo::availablePorts();
    if(infos.isEmpty())
    {
        er_msg = er_msg + " No Serial Ports Available...";
        //ui->textEdit->setText(er_msg);
        ui->label_comm->setText(er_msg);
        //ui->comboBox->addItem("Invalid");
        return;
    }


    //serial = new QSerialPort("COM4");       //Serial number, must match up, capital!!! Serial number to modify.

    serial->setPortName("/dev/ttyUSB0");
    serial->setBaudRate(QSerialPort::Baud9600);         //Baud rate
    serial->setDataBits(QSerialPort::Data8);            //Data bits
    serial->setParity(QSerialPort::NoParity);           //No parity
    serial->setStopBits(QSerialPort::OneStop);          //No stop bit
    serial->setFlowControl(QSerialPort::NoFlowControl); //No control

    if(serial->open(QIODevice::ReadWrite) )                 //Read write open
    {
        er_msg += ": Connected to ttyUSB0";
        //ui->textEdit->setText(er_msg);
        ui->label_comm->setText(er_msg);
    }
    else
    {
        er_msg += ": NOT Connected to ttyUSB0";
        //ui->textEdit->setText(er_msg);
        ui->label_comm->setText(er_msg);
    }

}

void MainWindow::sendSerialMsg(QString msg)
{
    if(msg.isEmpty())
    {
        return;
    }

    serial->write(msg.toLatin1());
}

//more complete read example here: http://www.programering.com/a/MTO2YjNwATM.html
void MainWindow::serialRead()
{
    //ui->textEdit->setText("");  //clear Text display
    ui->label_comm->setText("");

    QByteArray qba = serial->readAll();
    //ui->textEdit->append(qba);
    ui->label_comm->setText(qba);


    //QString sVal = QString::toStdString(qba);
    //ui->textEdit->append(serial->readAll());
}

#define Serial_Port_End }



#define Main_Timer_Loop_Start {
void MainWindow::timerEvent(QTimerEvent *event)
{
    static int frame_cnt = 0;

    cv::Mat image;


    //test is we're want live video
    if(!bLiveVideo)
    {
        if(!bProcessImage)  //process still image
        {
            return;
        }
    }

    myTimer.start();



//capture usb camera image
    if(bLiveVideo)
    {
        mCapture >> image;

        //chk if valid image

        int nrows = image.rows;
        int ncols = image.cols;
        int nchans = image.channels();

        if(nrows!=480)
        {
            return;
        }
        else if(ncols != 640)
        {
            return;
        }
        else if(nchans != 3)
        {
            return;
        }

    }
    else if(bProcessImage)
        image = image_copy.clone();



    //check if we're applying CLAHE filter
    if(bFltr_CLAHE)
    {
        cv::Mat lab_image;
        cv::cvtColor(image, lab_image, CV_BGR2Lab);

        // Extract the L channel
        std::vector<cv::Mat> lab_planes(3);
        cv::split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]

        // apply the CLAHE algorithm to the L channel
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
        clahe->setClipLimit(4);
        cv::Mat dst;
        clahe->apply(lab_planes[0], dst);

        // Merge the the color planes back into an Lab image
        dst.copyTo(lab_planes[0]);
        cv::merge(lab_planes, lab_image);

       // convert back to RGB
       cv::cvtColor(lab_image, image, CV_Lab2BGR);

    }


    //debate is should this come first or after previous image processing; like CLAHE histogram stretch...
    //for now after

     //modify image to reflect current zoom and pan settings
     process_zoomPan(image);  //this makes a copy "zoom_image" case we want to create a classifier from it that is free from point capture lines and painted pts



    num_objects_found = 0;
    num_pixels_classified = 0;

    if( bProcessPixels)  //process image check_box
    {

        //chk if we should be in HSV space
        if(bUseHSV)
        {
            process_image_HSV(image);
        }

        if(bUseBayes)
        {
            // use Bayes classifier, if it's built, loaded and ready to use
            if(classifier_ready_for_use)
            {
                use_classifier(image);
            }
            else
            {
                //flag error

            }

        }

    }


    //save a copy case we want to save it
    if(bLiveVideo)
        image_copy = image.clone();



    if(createClassifier_start)
    {
        //draw lines and points for current selector lines
        draw_region_lines(image);

        //draw rubber-band
        draw_rubber_band_lines(image);

        if(img_mouse_captured)
        {
            img_mouse_captured = false; //turn it back off


            if(img_mouse_right_captured)
            {
                img_mouse_right_captured = false;

                //we're done; turn this off
                rubber_band_on = false;
                mouse_capture_complete = true;

                img_mousePts_captured[++mouse_capture_count] = img_mousePts_captured[0];

                Point pt1,pt2;
                if(mouse_capture_count > 2)
                {
                    //use clean copy of image for final drawing
                    draw_image_region_lines(zoom_image);

                    //call next stage; i.e. build this region into trainingSet
                    //check if we really want to add this region,  via dialog box
                    //if so then;
                    process_classifier_capture_region(processing_image);  //need to use zoom_image (copy), so it doesn't have painted lines and points in image
                                                                    //zoom_image created by pan_zoom
                }
                else
                {
                    //skip processing and restart
                    //auto reload dialog box
                    cur_classId++;
                    on_actionCreate_Classifier_triggered();
                }

            }
            else
            {
                if(mouse_capture_complete == false)
                {
                    img_mousePts_captured[++mouse_capture_count] = img_mouse_lastCaptured;
                }
            }

            if(mouse_capture_count > 19)
            {
                QMessageBox msgBox;
                msgBox.critical(0, "Error", "Error to large a polygon; please restart... ");

                mouse_capture_count=0;
                return;

            }

        }

    }


    //convert image to QImage so we can display normally
    QImage qImg = Mat2QImage(image);


    ui->label_imageShow->setPixmap(QPixmap::fromImage(qImg));


     if(num_pixels_classified > 0)
    {
        QString msg = "$" + QString::number(num_objects_found) + "," + QString::number(num_pixels_classified) + "\n";
        sendSerialMsg(msg);
    }


    int nMilliseconds = myTimer.elapsed();

    skipCount++;
    if(skipCount > 10)
    {
        double fps = 0.0;
        if(nMilliseconds > 0)
        {
            fps = 1000.0/(double)nMilliseconds;
        }

        QString sInfo = "Mouse: (" + QString::number(img_mouse_pos.x) + ", " + QString::number(img_mouse_pos.y)  + "): Capture_time + Processing at (FPS): " + QString::number(fps,'f',1) + "  : Number Objects Found: " + QString::number(num_objects_found);
        ui->label_info->setText(sInfo);
        skipCount = 0;
    }
}

#define Main_Timer_Loop_End }



#define OpenCV_Utilities_Start {

void MainWindow::write_cvImage(cv::Mat &img, QString filePathName)
{
    QByteArray ba = filePathName.toLatin1();

    char* imageName = ba.data();

    int nrows = img.rows;

    if(nrows > 0)
        imwrite( imageName, img );
    else
    {
        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Error writing Image-File; Empty picture... ");
        return;
    }

}

void MainWindow::read_cvImage(cv::Mat &img, QString filePathName)
{
    QByteArray ba = filePathName.toLatin1();

    char* imageName = ba.data();

    //Mat img;
    img = imread( imageName, 1 );

}

//QImage qImg = Mat2QImage(image);
QImage MainWindow::Mat2QImage(const cv::Mat3b &src)
{
    QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
    for (int y = 0; y < src.rows; ++y)
    {
            const cv::Vec3b *srcrow = src[y];
            QRgb *destrow = (QRgb*)dest.scanLine(y);
            for (int x = 0; x < src.cols; ++x)
            {
                    destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
            }
    }
    return dest;
}

/*
QImage MainWindow::Mat2QImage(const cv::Mat_<double> &src)
{
    double scale = 255.0;
    QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
    for (int y = 0; y < src.rows; ++y)
    {
            const double *srcrow = src[y];
            QRgb *destrow = (QRgb*)dest.scanLine(y);
            for (int x = 0; x < src.cols; ++x)
            {
                    unsigned int color = srcrow[x] * scale;
                    destrow[x] = qRgba(color, color, color, 255);
            }
    }
    return dest;
}
*/

#define OpenCV_Utilities_End }



#define Pixel_Classifiers_Start {

void MainWindow::process_image_HSV(cv::Mat &img)
{
    Mat cImg = img.clone();
    //Mat cImg;

    // Convert BGR to HSV
    cvtColor(img, cImg, cv::COLOR_BGR2HSV);

/*  (values not same as GIMP!)

//according to GIMP (=2x opencv vals)
    //blue: H=240  SV for all of these = 100,100 so blue{240,100,100)
    //grn : H=120
    //red : H=0
    //Yel : H=60
    //Cyan: H=180

    //White: HSV = {0,0,100}
*/

    int hdelta = 15;
    int red_hdelta = 10;

    int blue = 120;
    int grn = 60;
    //int red = 170;
    int red = 160;
    int yellow = 30;
    int cyan = 90;


    int htv = ui->horizontalSlider_highThres->value();
    int ltv = ui->horizontalSlider_lowThres->value();

    int hue_htv = ui->horizontalSlider_hue_thresHigh->value();
    int hue_ltv = ui->horizontalSlider_hue_thresLow->value();


    if(bTuneS)
    {
        if(bTuneBlue)
        {
            low_blue_satVal = ltv;
            high_blue_satVal = htv;

            QString sLow = "Hue Blue Saturation (low): " + QString::number(low_blue_satVal);
            ui->label_LT->setText(sLow);

            QString sHigh = "Hue Blue Saturation (high): " + QString::number(high_blue_satVal);
            ui->label_HT->setText(sHigh);
        }
        else if(bTuneGreen)
        {
            low_grn_satVal = ltv;
            high_grn_satVal = htv;

            QString sLow = "Hue Green Saturation (low): " + QString::number(low_grn_satVal);
            ui->label_LT->setText(sLow);

            QString sHigh = "Hue Green Saturation (high): " + QString::number(high_grn_satVal);
            ui->label_HT->setText(sHigh);
        }
        else if(bTuneRed)
        {
            low_red_satVal = ltv;
            high_red_satVal = htv;

            QString sLow = "Hue Red Saturation (low): " + QString::number(low_red_satVal);
            ui->label_LT->setText(sLow);

            QString sHigh = "Hue Red Saturation (high): " + QString::number(high_red_satVal);
            ui->label_HT->setText(sHigh);
        }
        else if(bTuneRed2)
        {
            low_red2_satVal = ltv;
            high_red2_satVal = htv;

            QString sLow = "Hue Red Saturation (low): " + QString::number(low_red2_satVal);
            ui->label_LT->setText(sLow);

            QString sHigh = "Hue Red Saturation (high): " + QString::number(high_red2_satVal);
            ui->label_HT->setText(sHigh);
        }
        else if(bTuneYellow)
        {
            low_yellow_satVal = ltv;
            high_yellow_satVal = htv;

            QString sLow = "Hue Yellow Saturation (low): " + QString::number(low_yellow_satVal);
            ui->label_LT->setText(sLow);

            QString sHigh = "Hue Yellow Saturation (high): " + QString::number(high_yellow_satVal);
            ui->label_HT->setText(sHigh);
        }
        else if(bTuneCyan)
        {
            low_cyan_satVal = ltv;
            high_cyan_satVal = htv;

            QString sLow = "Hue Cyan Saturation (low): " + QString::number(low_cyan_satVal);
            ui->label_LT->setText(sLow);

            QString sHigh = "Hue Cyan Saturation (high): " + QString::number(high_cyan_satVal);
            ui->label_HT->setText(sHigh);
        }


    }
    else if(bTuneV)
    {
        low_val = ltv;
        high_val = htv;

        QString sLow = "Hue Value (low): " + QString::number(low_val);
        ui->label_LT->setText(sLow);

        QString sHigh = "Hue Value (high): " + QString::number(high_val);
        ui->label_HT->setText(sHigh);
    }

    if(bTuneYellow)
    {
        low_yellow_h = hue_ltv;
        high_yellow_h = hue_htv;

        QString sHTL = "Hue Yellow Threshold [20] (low): " + QString::number(low_yellow_h);
        QString sHTH = "Hue Yellow Threshold [40] (high): " + QString::number(high_yellow_h);

        ui->label_hueThres_high->setText(sHTH);
        ui->label_hueThres_low->setText(sHTL);
    }
    else if(bTuneBlue)
    {
        low_blue_h = hue_ltv;
        high_blue_h = hue_htv;

        QString sHTL = "Hue Blue Threshold [110] (low): " + QString::number(low_blue_h);
        QString sHTH = "Hue Blue Threshold [130] (high): " + QString::number(high_blue_h);

        ui->label_hueThres_high->setText(sHTH);
        ui->label_hueThres_low->setText(sHTL);
    }
    else if(bTuneRed)
    {
        low_red_h = hue_ltv;
        high_red_h = hue_htv;

        QString sHTL = "Hue Red-2 Threshold [150] (low): " + QString::number(low_red_h);
        QString sHTH = "Hue Red-2 Threshold [170] (high): " + QString::number(high_red_h);

        ui->label_hueThres_high->setText(sHTH);
        ui->label_hueThres_low->setText(sHTL);
    }
    else if(bTuneRed2)
    {
        low_red2_h = hue_ltv;
        high_red2_h = hue_htv;

        QString sHTL = "Hue Red-1 Threshold [0] (low): " + QString::number(low_red2_h);
        QString sHTH = "Hue Red-1 Threshold [10] (high): " + QString::number(high_red2_h);

        ui->label_hueThres_high->setText(sHTH);
        ui->label_hueThres_low->setText(sHTL);
    }
    else if(bTuneGreen)
    {
        low_grn_h = hue_ltv;
        high_grn_h = hue_htv;

        QString sHTL = "Hue Green Threshold [50] (low): " + QString::number(low_grn_h);
        QString sHTH = "Hue Green Threshold [70] (high): " + QString::number(high_grn_h);

        ui->label_hueThres_high->setText(sHTH);
        ui->label_hueThres_low->setText(sHTL);
    }
    else if(bTuneCyan)
    {
        low_cyan_h = hue_ltv;
        high_cyan_h = hue_htv;

        QString sHTL = "Hue Cyan Threshold [80] (low): " + QString::number(low_cyan_h);
        QString sHTH = "Hue Cyan Threshold [100] (high): " + QString::number(high_cyan_h);

        ui->label_hueThres_high->setText(sHTH);
        ui->label_hueThres_low->setText(sHTL);
    }


    Mat img_thres_blue, img_thres_red, img_thres_red2, img_thres_grn, img_thres_yellow, img_thres_cyan;


    cv::inRange(cImg, Scalar(low_blue_h,low_blue_satVal,low_val), Scalar( high_blue_h,high_blue_satVal,high_val),img_thres_blue);
    cv::inRange(cImg, Scalar(low_grn_h,low_grn_satVal,low_val), Scalar( high_grn_h,high_grn_satVal,high_val),img_thres_grn);
    cv::inRange(cImg, Scalar(low_red_h,low_red_satVal,low_val), Scalar( high_red_h,high_red_satVal,high_val),img_thres_red);
    cv::inRange(cImg, Scalar(low_red2_h,low_red2_satVal,low_val), Scalar( high_red2_h,high_red2_satVal,high_val),img_thres_red2);
    cv::inRange(cImg, Scalar(low_yellow_h,low_yellow_satVal,low_val), Scalar( high_yellow_h,high_yellow_satVal,high_val),img_thres_yellow);
    cv::inRange(cImg, Scalar(low_cyan_h,low_cyan_satVal,low_val), Scalar( high_cyan_h,high_cyan_satVal,high_val),img_thres_cyan);


    //reduce noise with medianBlur (note: only works on single channel images)
    //medianBlur(InputArray src, OutputArray dst, int ksize)  //ksize {3,5,7,9,....} (kernal-size)
    int ksize = 5;

    medianBlur(img_thres_blue, img_thres_blue, ksize);
    medianBlur(img_thres_grn, img_thres_grn, ksize);
    medianBlur(img_thres_red, img_thres_red, ksize);
    medianBlur(img_thres_red2, img_thres_red2, ksize);
    medianBlur(img_thres_yellow, img_thres_yellow, ksize);
    medianBlur(img_thres_cyan, img_thres_cyan, ksize);

    //reduce noise with erosion
    /*
    Mat b_dilate, b_erode;
    int niters = 3;

    Mat binary_dst = img_thres_blue.clone();
    dilate(binary_dst,  b_dilate, Mat(), Point(-1,-1), niters);
    erode( b_dilate, b_erode, Mat(), Point(-1,-1), niters*2);
    dilate(b_erode, binary_dst, Mat(), Point(-1,-1), niters);

    img_thres_blue = binary_dst;

*/


    int blue_cnt = cv::countNonZero(img_thres_blue);
    int red_cnt = cv::countNonZero(img_thres_red);
    int red2_cnt = cv::countNonZero(img_thres_red2);
    int grn_cnt = cv::countNonZero(img_thres_grn);
    int yellow_cnt = cv::countNonZero(img_thres_yellow);
    int cyan_cnt = cv::countNonZero(img_thres_cyan);

    num_pixels_classified += blue_cnt;
    num_pixels_classified += red_cnt;
    num_pixels_classified += red2_cnt;
    num_pixels_classified += grn_cnt;
    num_pixels_classified += yellow_cnt;
    num_pixels_classified += cyan_cnt;


    //use binary classed image as a mask and set all white pixels in mask to spec'd color in color img
    //zero out img first
    if(bClearBackground)
    {
        img.setTo(Scalar(0,0,0));
    }

    //set all high thresholded pixels to blue (if we don't zero out img; you'll have normal img with the recognized pixels in blue
    img.setTo(Scalar(255,0,0),img_thres_blue);
    img.setTo(Scalar(0,255,0),img_thres_grn);
    img.setTo(Scalar(0,0,255),img_thres_red);
    img.setTo(Scalar(0,0,255),img_thres_red2);
    img.setTo(Scalar(0,255,255),img_thres_yellow);
    img.setTo(Scalar(255,255,0),img_thres_cyan);



    if(bObjectProcess)
    {
        //for blob analysis merge all
        //merge
        Mat classed_all, classed_all12, classed_all34;
        bitwise_or(img_thres_blue, img_thres_red, classed_all12 );
        bitwise_or(img_thres_yellow, img_thres_grn, classed_all34 );
        bitwise_or(classed_all34, img_thres_cyan, classed_all34 );
        bitwise_or(classed_all12, classed_all34, classed_all );


        double threshold_value_high = 120;
        double threshold_value_low = 90;
        double max_BINARY_value = 1.0;
        int threshold_type = 0;

        /* 0: Binary
           1: Binary Inverted
           2: Threshold Truncated
           3: Threshold to Zero
           4: Threshold to Zero Inverted
         */

       //threshold( src_gray, dst, threshold_value, max_BINARY_value,threshold_type );
       //double threshold(InputArray src, OutputArray dst, double thresh, double maxval, int type)
       threshold( classed_all, classed_all, threshold_value_high, max_BINARY_value,threshold_type );

       Mat binary_dst, b_dilate, b_erode;

       int niters = 3;
       dilate(classed_all,  b_dilate, Mat(), Point(-1,-1), niters);
       erode( b_dilate, b_erode, Mat(), Point(-1,-1), niters*2);
       dilate(b_erode, binary_dst, Mat(), Point(-1,-1), niters);


        //clear out cv_img
           //image = cv::Scalar(120,220,120);

           std::vector <cv::Point2d> blob_centers_all;
           std::vector < std::vector<cv::Point2i > > blobs;

           cObjClasser->FindBlobs(binary_dst, blobs, blob_centers_all);

           //use slider for this?
           int MIN_BLOB_SIZE=10;

           // Randomy color the blobs
           int N = blobs.size();

           num_objects_found = N;


           for(size_t i=0; i < N; i++)
           {
               ///unsigned char r = 255 * (rand()/(1.0 + RAND_MAX));
               ///unsigned char g = 255 * (rand()/(1.0 + RAND_MAX));
               ///unsigned char b = 255 * (rand()/(1.0 + RAND_MAX));
               unsigned char r = 255 ;
               unsigned char g = 255 ;
               unsigned char b = 255 ;

               int M = blobs[i].size();

               if(M>MIN_BLOB_SIZE)
               {
                   for(size_t j=0; j < blobs[i].size(); j++)
                   {
                       int x = blobs[i][j].x;
                       int y = blobs[i][j].y;

                       img.at<cv::Vec3b>(y,x)[0] = b;
                       img.at<cv::Vec3b>(y,x)[1] = g;
                       img.at<cv::Vec3b>(y,x)[2] = r;
                   }
               }

           }

           //this is what we were doing
           //cvtColor( classed_all, img, CV_GRAY2BGR );

    }



}

#define Pixel_Classifiers_End }





//should only be here if region has been captured and confirmed via ok on dialog box
void MainWindow::process_classifier_capture_region(Mat &img)
{
    run_histogram = true;

    vector<Point> captured_pts;
    int npts = mouse_capture_count;

    for(int i=0;i<npts;i++)
    {
        captured_pts.push_back(img_mousePts_captured[i]);
    }

//test ok
//    imshow("test image in MainWin",img);
//return;

    //really should add a bool ok here
    //selection_rect
    cclass->add_trainingData_to_dataSet(img, captured_pts, cur_classId, run_histogram, selection_rect);

    //auto reload dialog box
    cur_classId++;
    on_actionCreate_Classifier_triggered();

}



//process current image using currently define classifier
void MainWindow::use_classifier(Mat &img)
{
    //make sure we can use it as we're mixing * & for passing image..., should really fix this to make it all consistent...
    //imshow("Test at: Use classifier Image",img);

    cclass->process_image_3dLut(img);
    //cclass->process_image_3dLut(zoom_image);
    //cclass->process_image(img);  //process image using currently defined classifier [need to move our current processing into classifier class, or make another one...]
}


// ** ON ZOOM: Add Feature that IFF a set of classifier region pts are defined (not closed yet), then clicking
// on '+' zoom button, will zoom into a bounding box surrounding those points.
// use can then hit Escape to reset mouse region capture, to do so more optimally on a pre-zoom area.



//modify input image to reflect current zoom and pan settings
//output will be a 640x480 image
void MainWindow::process_zoomPan(Mat &img)
{
  Mat imageROI;

    Point output_imageSize;
    output_imageSize.x = DISPLAY_PIXEL_WIDTH;
    output_imageSize.y = DISPLAY_PIXEL_HEIGHT;

    cocv->process_zoomPan(img, zoom_factor, pan_topLeft, output_imageSize);

    zoom_image = img.clone();
    processing_image = img.clone();
}


void MainWindow::load_image()
{
    //cclass->test5();
    //cclass->test4();    //chk 3 color classifier
    //cclass->test6();  //quad six-color_image and save
    cclass->test7();    //mlp example
    //cclass->build_classifier();
//return;

    bLiveVideo = false;
    ui->action_Live_Video->setChecked(false);

    QString pics_path = QStandardPaths::locate(QStandardPaths::PicturesLocation, QString(), QStandardPaths::LocateDirectory);

    QFileDialog qfd;

    qfd.setDirectory(pics_path);

    //get save path
    QString filePathName = qfd.getOpenFileName(this,tr("Open Image"),"",tr("Image (*.png);Image (*.jpg);All Files (*)"));
    //QString filePathName = qfd.getSaveFileName(this, tr("Save Image"), "", tr("Image (*.png);Image (*.jpg);All Files (*)"));


    if (filePathName.isEmpty())
        return;

    load_image_from_file(filePathName);
}

void MainWindow::load_image_from_file(QString filePathName)
{
    //need to test ext to make sure it's png or jpg (valid image extension
    QList<QString> ext_list;
    ext_list<<"bmp"<<"jpg"<<"png";
    QFileInfo fi(filePathName);
    QString ext = fi.suffix();
    if (ext_list.contains(ext))
    {
        read_cvImage(image_copy, filePathName);  //so, we're going to read this into image_copy; how to let process know to use it?

        bProcessImage = true;
        //ui->action_Open_Image->setChecked(true);
        ui->action_Process_Image_as_Live->setChecked(true);

        ui->action_Process_Image_as_Live->setEnabled(true);

        if(!loopTimerStarted)
        {
            loopTimerStarted = true;
            startTimer(10); //want to be able to treat image as live-video; question will be how to switch to live video from still image...?
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.critical(0, "Error", "You must select a valid image file (ensure extension is {*.png, *.jpg, *.bmp}");
        return;
    }

}

void MainWindow::draw_region_lines(cv::Mat &img)
{
    for(int i=0;i<=mouse_capture_count;i++)
    {
        Point center = img_mousePts_captured[i];
        cocv->drawPoint(&img, center, cv::Scalar(0,0,255));

        if(i>0)
        {
            Point pt2 = img_mousePts_captured[i-1];
            cocv->drawLine(img,center,pt2,cv::Scalar(0,255,0));
        }
    }

}

void MainWindow::draw_rubber_band_lines(cv::Mat &img)
{
    if(rubber_band_on)
    {
        if(mouse_capture_count >= 0)
        {
            //draw rubber-band from last point to where mouse currently is
            Point last_pt = img_mousePts_captured[mouse_capture_count];
            cocv->drawLine(img,img_mouse_pos,last_pt,cv::Scalar(0,0,255));
        }
    }

}

void MainWindow::draw_image_region_lines(cv::Mat img)
{
    draw_region_lines(img);

    //convert image to QImage so we can display normally

    QImage qImg = Mat2QImage(img);
    ui->label_imageShow->setPixmap(QPixmap::fromImage(qImg));

    Rect rect;
    get_rect_from_region(rect);

}

void MainWindow::get_rect_from_region(Rect &rect)
{
    int left=99999,bottom=99999,right=0,top=0,height,width;

    for(int i=0;i<=mouse_capture_count;i++)
    {
        Point pt = img_mousePts_captured[i];
        int x = pt.x;
        int y = pt.y;

        if(left>x)
        {
            left = x;
        }
        if(bottom>y)
        {
            bottom = y;
        }
        if(right<x)
        {
            right = x;
        }
        if(top<y)
        {
            top = y;
        }

    }

    width = right - left;
    height = top - bottom;

    rect = Rect(left,bottom,width,height);

    selection_rect = rect;  //save it for processing

    //zoom-in while preserving aspect ratio
    //expand_rect_to_preserve_aspectRatio(rect);


}

void MainWindow::expand_rect_to_preserve_aspectRatio(Rect &rect)
{
    double desired_aspectRatio = 640.0/480.0;

    const int MAX_WIDTH = 640;
    const int MAX_HEIGHT = 480;

    int width, height, dx, dy;

    //see if we need to expand width or height
    double aspectRatio = (double)rect.width/(double)rect.height;

    if(aspectRatio < desired_aspectRatio)
    {
        height = rect.height;

        //need to expand width
        //compute correct size expansion
        double exp_factor = desired_aspectRatio/aspectRatio;

        width = (double)rect.width * exp_factor;

        dx = width - rect.width;

        dx = dx/2;

        rect.x = rect.x - dx;
        if(rect.x < 0)
            rect.x = 0;

        int max_x = rect.x + width;

        if(max_x < MAX_WIDTH)
        {
            rect.width = max_x;
        }
        else
        {
            MAX_WIDTH - rect.y;
        }

    }
    else
    {
        width = rect.width;

        //need to expand height
        double exp_factor = aspectRatio/desired_aspectRatio;

        int height = (double)rect.width * exp_factor;

        dy = height - rect.height;
        dy = dy/2;

        rect.y = rect.y - dy;

        if(rect.y < 0)
            rect.y = 0;

        int max_y = rect.y + height;

        if(max_y < MAX_HEIGHT)
        {
            rect.height = max_y;
        }
        else
        {
            rect.height = MAX_HEIGHT - rect.y;
        }

    }

    double new_aspectRatio = (double)rect.width/(double)rect.height;

    double z = new_aspectRatio;

}
