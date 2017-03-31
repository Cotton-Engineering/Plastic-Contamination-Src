#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Main User Gui Dialog


#include <QMainWindow>

#include <QTime>
#include <QTimer>

#include <QtSerialPort/QtSerialPort>
#include <QSerialPortInfo>

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

#include <iostream>
#include <math.h>
#include <string.h>



#include "dialog_create_classifier.h"
#include "copencv_utils.h"
#include "cclassifier.h"
#include "cobjectclassifier.h"



//#include "flycapture/FlyCapture2.h"
//using namespace FlyCapture2;
using namespace cv;
using namespace std;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void Mouse_current_pos();
    void Mouse_Pressed();
    void Mouse_Right_Pressed();
    void Mouse_left();
    void Mouse_Released();
    void Mouse_Right_Released();


    void on_action_Start_triggered();

    void on_actionImage_Save_triggered();

    void on_action_Open_Image_triggered();

    void on_action_Process_Image_as_Live_triggered(bool checked);

    void on_action_Process_Image_triggered(bool checked);

    void on_action_Object_Process_triggered(bool checked);

    void on_actionUse_HSV_triggered(bool checked);

    void on_action_Live_Video_triggered(bool checked);

    void on_action_Clear_Background_triggered(bool checked);

    void on_actionTune_Hue_Blue_triggered(bool checked);

    void on_actionTune_Hue_Yellow_triggered(bool checked);

    void on_actionTune_Hue_Green_triggered(bool checked);

    void on_actionTune_Hue_Red_triggered(bool checked);

    void on_actionTune_Hue_Cyan_triggered(bool checked);

    void on_actionTune_Hue_Saturation_triggered(bool checked);

    void on_actionTune_Hue_Value_triggered(bool checked);

    void serialRead();

    void loadSettings();
    void saveSettings();




    void on_actionSave_Pixel_Class_Values_triggered();

    void on_actionTune_Hue_Red2_triggered(bool checked);

    void on_actionCLAHE_Filter_triggered(bool checked);

    void on_actionCreate_Classifier_triggered();

    void on_actionUse_Bayes_Classifier_triggered(bool checked);

private:
    Ui::MainWindow *ui;

    void initSerialPort();
    void sendSerialMsg(QString msg);

    void read_cvImage(cv::Mat &img, QString filePathName);
    void write_cvImage(cv::Mat &img, QString filePathName);

    //QImage Mat2QImage(const cv::Mat_<double> &src);
    QImage Mat2QImage(const cv::Mat3b &src);


    void draw_image_region_lines(cv::Mat img);

    void draw_region_lines(cv::Mat &img);
    void draw_rubber_band_lines(cv::Mat &img);

    void get_rect_from_region(Rect &rect);
    void expand_rect_to_preserve_aspectRatio(Rect &rect);


    void process_image_HSV(cv::Mat &img);  //note: use of '&' means reference, so function is allowed to change it; allows for stronger type checking than a pointer, '*'
    void turn_off_all_HUV_tuners();

    void init_capture();
    void clear_mouse_capture_buffer();

//    void build_classifier();  //add done adding regions, so build it and make it avail
//    void clear_classifier();  //clear classifier; start over basically, remove any regions already added to trainingSet
    //void clear_classifier(int classId);  //not needed; you can just select "new" instead of append when creating class

    void use_classifier(Mat &img);  //process current image using currently define classifier

//    void save_classifier(QString sFilePathName, QString sHeaderLine, bool append);
//    void save_trainingSet(QString sFilePathName, QString sHeaderLine, bool append);

    void process_classifier_capture_region(Mat &img);

    void process_zoomPan(Mat &img);  //modify input image to reflect current zoom and pan settings

    void load_image_from_file(QString filePathName);
    void load_image();


protected:
    void timerEvent(QTimerEvent *event);
    void keyPressEvent( QKeyEvent* event ) ;

private:
    QString sInit_fileName;

    QSettings * p_settings;

    cv::VideoCapture mCapture;  //standard usb camera
    cv::Mat image_copy;
    cv::Mat zoom_image;
    cv::Mat processing_image;


    Dialog_create_classifier *dcc;


    cOpenCv_Utils *cocv;

    cClassifier *cclass;
    cObjectClassifier *cObjClasser;

    bool loopTimerStarted;

    bool bProcessPixels;
    bool bObjectProcess;
    bool bUseHSV;
    bool bUseBayes=false;
    bool bLiveVideo;
    bool bClearBackground;
    bool bTuneYellow;
    bool bTuneBlue;
    bool bTuneRed;
    bool bTuneRed2;
    bool bTuneGreen;
    bool bTuneCyan;
    bool bProcessImage;

    bool bTuneS;
    bool bTuneV;

    bool bFltr_CLAHE;

    QSerialPort *serial;

    bool first_time;

    QTime myTimer;

    int skipCount;

    int num_objects_found;
    int num_pixels_classified;

    int low_satVal;
    int high_satVal;

    int low_blue_satVal;
    int high_blue_satVal;

    int low_grn_satVal;
    int high_grn_satVal;

    int low_red_satVal;
    int high_red_satVal;

    int low_red2_satVal;
    int high_red2_satVal;

    int low_yellow_satVal;
    int high_yellow_satVal;

    int low_cyan_satVal;
    int high_cyan_satVal;

    int low_val;
    int high_val;

    int low_blue_h;
    int high_blue_h;
    int low_grn_h;
    int high_grn_h;
    int low_red_h;
    int high_red_h;
    int low_red2_h;
    int high_red2_h;
    int low_yellow_h;
    int high_yellow_h;
    int low_cyan_h;
    int high_cyan_h;

    Point img_mouse_pos;
    Point img_mouse_lastCaptured;
    Point img_mouse_last1Captured;
    Point img_mouse_last2Captured;
    static const int MOUSE_CAPTURE_BUFFER_SIZE = 20;
    Point img_mousePts_captured[MOUSE_CAPTURE_BUFFER_SIZE];
    Rect selection_rect;
    bool img_mouse_captured;
    bool img_mouse_right_captured;
    int mouse_capture_count;
    bool mouse_firstPress = true;
    bool mouse_right_firstPress = true;
    bool rubber_band_on = true;
    bool mouse_capture_complete = false;

    bool createClassifier_start = false;
    bool run_build_classifier = false;
    bool run_histogram = false;

    //    bool run_clear_classifier = false;
    int cur_classId = 0;
    bool classifier_ready_for_use = false;
//    bool append_to_existing_class = true;
//    bool append_to_existing_trainingSet = true;



//    bool save_current_trainingSet = false;
//    bool save_current_classifier = false;
//   bool load_trainingSet_fromFile = false;

//    bool save_to_file = true;
//    bool fileNameLoaded = false;
//    QString filePathName;
//    QString sHeaderLine;




    // PAN/ZOOM
    Point pan_center, pan_topLeft;
    int zoom_factor=1;
    bool isZoomed=false;
    bool use_panCenter = false;

    static const int DISPLAY_PIXEL_WIDTH = 640;
    static const int DISPLAY_PIXEL_HEIGHT = 480;
};

#endif // MAINWINDOW_H
