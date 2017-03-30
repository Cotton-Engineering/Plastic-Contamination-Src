#ifndef CCLASSIFIERDATA_H
#define CCLASSIFIERDATA_H

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


using namespace cv;
using namespace cv::ml;
using namespace std;


class cClassifierData : public QObject
{
    Q_OBJECT
public:
    explicit cClassifierData(QObject *parent = 0);

signals:

public slots:

public:

    void add_pixelData(Point pt, int classId );
    //void add_pixelData(Point3i pt, int classId );
    void add_pixelData(Point3i bgr_pt, Point3i hsv_pt, int classId );

    bool load_trainingData(vector<Point> &trainData, vector<int> &trainLabels);
    bool load_trainingData(vector<Point3i> &bgr_trainData, vector<Point3i> &hsv_trainData, vector<int> &trainLabels);

    bool save_current_trainingData_to_csv(QString filePathName, QString headerLine, bool append=false);
    bool save_current_3i_trainingData_to_csv(QString filePathName, QString headerLine, bool append=false);

    bool read_csv_trainingData(QString filePathName, vector<Point> &trainData, vector<int> &trainLabels);
    bool read_3i_csv_trainingData(QString filePathName, vector<Point3i> &bgr_trainData, vector<Point3i> &hsv_trainData, vector<int> &trainLabels);


    //returns data in format ready for submission to classifiers
    bool get_mat_from_list(Mat &tDataMat, Mat &tLabelsMat);
    bool get_mat_from_list(vector<Point3i> &trainData3i, vector<int> &trainLabels3i, Mat &tDataMat, Mat &tLabelsMat);


    bool get_classId_list(vector<int> &classId_list);
    bool get_classId_3i_list(vector<int> &classId_list);

    bool get_random_trainingTestSet(vector<Point> &trainData, vector<int> &trainLabels, vector<Point> &testData, vector<int> &testLabels);
    bool get_random_3i_trainingTestSet(vector<Point3i> &trainData3i, vector<int> &trainLabels3i, vector<Point3i> &testData3i, vector<int> &testLabels3i);

    bool get_random_3i_trainingTestSet(int classId, vector<Point3i> &trainData3i, vector<int> &trainLabels3i, vector<Point3i> &testData3i, vector<int> &testLabels3i);
    bool get_random_trainingTestSet(int classId, vector<Point> &trainData, vector<int> &trainLabels, vector<Point> &testData, vector<int> &testLabels);

    void append_testSet(vector<Point> &newTrainData, vector<int> &newTrainLabels, vector<Point> &trainData, vector<int> &trainLabels);
    void append_3i_testSet(vector<Point3i> &newTrainData3i, vector<int> &newTrainLabels3i, vector<Point3i> &trainData3i, vector<int> &trainLabels3i);

    void clear_class_from_list(int classId);
    void clear_all();

    long unsigned int length=0;

    bool use_three_colors = true;

private:

    bool is_pixelList_biModal(vector<Point> pt_list);
    bool is_pixelList_biModal(vector<Point3i> pt_list);
    bool split_biModal(vector<Point> pt_list, vector<Point> &pt_list_a, vector<Point> &pt_list_b);
    bool split_biModal(vector<Point3i> pt_list, vector<Point3i> &pt_list_a, vector<Point3i> &pt_list_b);


    bool remove_items_from_pt_list(int start_indx, int stop_indx);
    bool remove_items_from_pt3i_list(int start_indx, int stop_indx);

    bool remove_item_from_pt_list(int indx);
    bool remove_item_from_pt3i_list(int indx);

    void sort_class_lists();
    void sort_class_3iLists();

    //note: this will also sort master lists
    bool find_first_last_indx_classId(int classId, int &start_indx, int &stop_indx);
    bool find_first_last_3i_indx_classId(int classId, int &start_indx, int &stop_indx);

    bool get_data_by_classId(int classId, vector<int> &label_list, vector<Point> &pt_data_list );
    bool get_data_by_3iclassId(int classId, vector<int> &label_list, vector<Point3i> &pt_data_list );

private:

    vector<Point> master_data_list;
    vector<int> master_label_list;

    vector<Point3i> master_data3i_list;
    vector<int> master_label3i_list;

    vector<Point3i> master_bgr_data3i_list;

};

#endif // CCLASSIFIERDATA_H
