#ifndef COBJECTCLASSIFIER_H
#define COBJECTCLASSIFIER_H

#include <QObject>

#include <QString>

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/ml.hpp"



#include <vector>

#include <iostream>
#include <math.h>
#include <string.h>

#include "copencv_utils.h"
#include "cclassifierdata.h"


using namespace cv;
using namespace cv::ml;
using namespace std;




class cObjectClassifier : public QObject
{
    Q_OBJECT

    struct OBJECT_FEATURES
    {
        vector<double> contour_areas;
        vector<RotatedRect> rotRect_list;
        vector<RotatedRect> rotEllipse_list;
        vector<double> perimeters;
        vector<double> perimeter_area_ratios;
        vector<double> inertias;
        vector<double> convexity_ratios;
        vector<double> hull_areas;
        vector<double> moment_areas;
    };

public:
    explicit cObjectClassifier(QObject *parent = 0);

    void FindBlobs(const cv::Mat &binary, std::vector < std::vector<cv::Point2i> > &blobs, std::vector <cv::Point2d> &blobCenters);

    bool getContours(cv::Mat &binary, vector<vector<Point> > &contours, vector<Vec4i> &hierarchy);

    bool drawContours(cv::Mat &binary, vector<vector<Point> > &contours, vector<Vec4i> &hierarchy);

    bool getObjectFeatures(vector<vector<Point> > &contours, OBJECT_FEATURES &obj_features);



signals:

public slots:

private:


};

#endif // COBJECTCLASSIFIER_H
