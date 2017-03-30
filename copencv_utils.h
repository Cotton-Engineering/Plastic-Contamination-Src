#ifndef COPENCV_UTILS_H
#define COPENCV_UTILS_H

#include <QObject>

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

#include <iostream>
#include <fstream>
#include <math.h>
#include <string.h>

#include "cutils.h"


using namespace cv;
using namespace std;


class cOpenCv_Utils : public QObject
{
    Q_OBJECT
public:
    explicit cOpenCv_Utils(QObject *parent = 0);

    QString convert_point_to_csv_QString(Point pt);
    QString convert_point3i_to_csv_QString(Point3i pt);


    void drawRedLine(Mat img, Point start, Point end);
    void drawGreenLine(Mat img, Point start, Point end);
    void drawRedFilledPoly(Mat img, const Point **pp_pts, int npts);
    void drawGreenFilledPoly(Mat img, const Point **pp_pts, int npts);
    void drawGreenPoint(Mat img, Point center);
    void drawRedPoint(Mat img, Point center);

    void drawPoint(Mat *img, Point center, cv::Scalar color);
    void drawLine(Mat img, Point start, Point end, cv::Scalar color);
    void drawFilledPoly(Mat img, const Point **pp_pts, int npts, cv::Scalar color);

    void plot_hsv_histogram( Mat img, Mat mask  );
    void plot_hs_histogram(Mat src, Mat mask );

    void set_color(Mat &img, int row, int col, Vec3b clr);
    Vec3b get_color(Mat img, int row, int col);
    Vec3b get_color_hsv(Mat img, int row, int col);

    vector<Vec3b> convert_Mat_to_vector_vec3b(Mat img);
    vector<Vec3b> convert_Mat_to_vector_vec3b(Mat img, int row, int col, int width, int height);
    vector<Vec3b> convert_Mat_to_vector_vec3b(Mat img, Rect rect);

    bool copy_srcMatRect_to_dstMatRect(Mat miniMat_src, Mat &dstMat, Rect srcRect, Rect dstRect);

    Mat create_color_mat(Rect imgRect, Vec3b clr);
    Mat create_color_mat(int width, int height, Vec3b clr);
    Mat create_color_mat(int width, int height, uchar blueVal, uchar greenVal, uchar redVal);
    Mat create_color_mat(Rect imgRect, uchar blueVal, uchar greenVal, uchar redVal);

    //single channel paint poly into otherwise black image
    void createMask_from_poly(Mat img, Mat *img_mask, vector<Point> mask_pts);

    //extract a subset image, spec'd by roi
    Mat get_Mat_from_imageROI(Mat img, Rect roi);
    Mat set_image_roi(Mat img, Rect region_of_interest);
    void process_zoomPan(Mat &img, int zoom_factor, Point pan_topLeft, Point output_image_size);


    //paint poly onto copy of existing image
    void paint_poly_to_image(Mat src, Mat *dest, vector<Point> mask_pts, cv::Scalar clr);

    Mat norm_0_255(InputArray _src);
    //void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';');
    int writeMatToFile(const Mat &I, string path);
    int readFileToMat(Mat &I, string path);

    void saveImage(Mat img);


    double get_dist_2Points(Point pt1, Point pt2);

    cv::Mat shuffleRows(const cv::Mat &matrix);

    int knn_getBestSelection(Mat neighborsClassList, int MAX_NUM_CLASSES);
    //bool sort_knnBest (const cv::Point p1, const cv::Point p2);


signals:

public slots:

private:

};

#endif // COPENCV_UTILS_H
