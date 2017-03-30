#ifndef CCLASSIFIER_H
#define CCLASSIFIER_H

#include <QObject>
#include <QString>

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/ml.hpp"



#include <iostream>
#include <math.h>
#include <string.h>

#include "copencv_utils.h"
#include "cclassifierdata.h"


using namespace cv;
using namespace cv::ml;
using namespace std;


class cClassifier : public QObject
{
    Q_OBJECT
public:
    explicit cClassifier(QObject *parent = 0);

private:
    // eval. accuracy
    float evaluate(cv::Mat& predicted, cv::Mat& actual);
    void plot_binary(cv::Mat& data, cv::Mat& classes, string name);


    //void knn(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses, int K);
    void bayes(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses);
    //void decisiontree(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses);

    void read_cvImage(cv::Mat &img, QString filePathName);

signals:

public slots:

public:
    int test();
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();
    void test7();

    bool save_current_trainingSet(QString filePathName, QString headerLine, bool append=true);
    bool save_current_3i_trainingSet(QString filePathName, QString headerLine, bool append=true);
    bool load_toCurrent_trainingSet(QString filePathName);
    bool load_toCurrent_3i_trainingSet(QString filePathName);


    cv::Mat load_data_from_csv(QString filePathName, int headerLineCount, int class_id);  //headerLinecount is # of lines to skip at top of file
    cv::Mat load_data_from_image(QString filePathName, int class_id);

    //vector<Point2f> convert_mat_to_point_array(Mat a);
    //vector<Point3f> convert_mat_to_point_array(Mat a);
    QString get_openCv_type(int type);

    void load_Bayes_Classifier(QString fileName);
    void save_Bayes_Classifier(QString fileName, Mat trainingData, Mat trainingLabels);

    void save_TrainingSet(QString fileName, Mat trainingData, Mat trainingLabels);
    void load_TrainingSet(QString fileName, Mat &trainingData, Mat &trainingLabels);


    //build dataSets
    void add_trainingData_to_dataSet(Mat img, vector<Point> mask_pts, int classId, bool run_histogram, Rect selection_rect);
    void extractTrainingData(Mat img, vector<Point> mask_pts, int classId, Rect selection_rect);
    void build_trainingSet(Mat img, Mat mask, int classId, Rect selection_rect);

    bool build_classifier();  //returns results of build; if successful, and classifier's ready to use, returns true
    void clear_classifier();

    //void use_classifier(Mat &img);

    bool build_lut();
    void build_lut_knn();
    void build_lut_bayes();
    void build_lut_svm();

    void build_3dlut_bayes();


    void display_lut();
    void process_image_lut(Mat &img);  //main processing routine
    void process_image_3dLut(Mat &img);

    void process_image(Mat &img); //not using this; using process_image_lut instead

    //void plot_hsv_histogram( Mat img, Mat mask );
    //void plot_hs_histogram(Mat src, Mat mask );

    int classifier_type_to_use = 1;  //1)bayes, 2)Knn, 3)Svm, 4) ...
    bool classifier_ready_to_use = false;

    bool use_LUT_to_classify_image = false;

    bool use_three_colors = true;

private:
    cOpenCv_Utils *cocv;

    Ptr<ml::NormalBayesClassifier> ptr_bayesCl;
    bool isBayesTrained = false;

    Ptr<ml::KNearest> ptr_knn;
    bool isKnnTrained = false;

    Ptr<ml::SVM> ptr_svm;
    bool isSvmTrained = false;


    Mat master_trainData, master_trainLabels, hs_Lut;
    //vector<Mat> hs_3dLut;
    //vector<Mat> hs_3dLut(256,Mat(256,256,CV_8UC1));
    //vector<vector<Point3i>> hs_3dLut;
    unsigned char hs_3dLut[256][256][256];


    cClassifierData master_ccd, train_ccd, test_ccd;


    bool plotSupportVectors = true;
    int numTrainingPoints = 200;
    int numTestPoints = 2000;
    int size = 200;
    int eq = 0;
};

#endif // CCLASSIFIER_H
