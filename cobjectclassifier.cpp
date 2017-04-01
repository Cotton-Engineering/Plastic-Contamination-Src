#include "cobjectclassifier.h"

cObjectClassifier::cObjectClassifier(QObject *parent) : QObject(parent)
{

}

bool cObjectClassifier::drawContours(cv::Mat &img, vector<vector<Point> > &contours, vector<Vec4i> &hierarchy)
{
    bool ok = false;

    int levels = 3;


    int w = img.cols;
    int h = img.rows;
    int nChans = img.channels();

    if(nChans != 3)
    {
        return false;
    }

    Mat cnt_img = Mat::zeros(h, w, CV_8UC3);

    int _levels = levels - 3;
    cv::drawContours( cnt_img, contours, _levels <= 0 ? 3 : -1, Scalar(128,255,255),3, LINE_AA, hierarchy, std::abs(_levels) );

    imshow("contours", cnt_img);


    ok = true;

    return ok;
}


bool cObjectClassifier::getObjectFeatures(vector<vector<Point> > &contours, OBJECT_FEATURES &obj_features)
{
    const double eps = 1e-2;


    //vector<double> areas;
    //vector<double> perimeters;
    //vector<double> ratios;
    //vector<double> inertias;
    //vector<double> convexities;

    double moment_area, perimeter, perimeter_area_ratio, inertia, convexity_ratio, contour_area, hull_area;
    double radius;

    //get best fit rotated rectangles and ellipses
    vector<RotatedRect> rotRect_list(contours.size());
    vector<RotatedRect> rotEllipse_list(contours.size());

    RotatedRect nullRect();

    for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++)
    {
        rotRect_list[contourIdx] = minAreaRect(Mat(contours[contourIdx]));


        if(contours.size() > 5)
        {
            rotEllipse_list[contourIdx] = fitEllipse(Mat(contours[contourIdx]));
        }
    }

    obj_features.rotRect_list = rotRect_list;
    obj_features.rotEllipse_list = rotEllipse_list;


    //Get rest of geometry features
    for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++)
    {
        Moments moms = moments(Mat(contours[contourIdx]));
        moment_area = moms.m00;
        obj_features.moment_areas.push_back(moment_area);

        perimeter = arcLength(Mat(contours[contourIdx]), true);
        obj_features.perimeters.push_back(perimeter);

        perimeter_area_ratio = 4 * CV_PI * moment_area / (perimeter * perimeter);
        obj_features.perimeter_area_ratios.push_back(perimeter_area_ratio);

        double denominator = std::sqrt(std::pow(2 * moms.mu11, 2) + std::pow(moms.mu20 - moms.mu02, 2));
        if(denominator > eps)
        {
            double cosmin = (moms.mu20 - moms.mu02) / denominator;
            double sinmin = 2 * moms.mu11 / denominator;
            double cosmax = -cosmin;
            double sinmax = -sinmin;

            double imin = 0.5 * (moms.mu20 + moms.mu02) - 0.5 * (moms.mu20 - moms.mu02) * cosmin - moms.mu11 * sinmin;
            double imax = 0.5 * (moms.mu20 + moms.mu02) - 0.5 * (moms.mu20 - moms.mu02) * cosmax - moms.mu11 * sinmax;
            inertia = imin / imax;

        }
        else
        {
            inertia = 0.0;
        }

        obj_features.inertias.push_back(inertia);


        std::vector < Point > hull;
        convexHull(Mat(contours[contourIdx]), hull);
        contour_area = contourArea(Mat(contours[contourIdx]));
        hull_area = contourArea(Mat(hull));
        convexity_ratio = contour_area / hull_area;

        obj_features.contour_areas.push_back(contour_area);
        obj_features.hull_areas.push_back(hull_area);
        obj_features.convexity_ratios.push_back(convexity_ratio);



    }

}

bool cObjectClassifier::getContours(cv::Mat &binaryImg, vector<vector<Point> > &contours, vector<Vec4i> &hierarchy)
{
    bool ok = false;

    int nChans = binaryImg.channels();

    if(nChans != 1)
    {
        return false;
    }


    vector<vector<Point> > contours0;

    findContours( binaryImg, contours0, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    contours.resize(contours0.size());
    for( size_t k = 0; k < contours0.size(); k++ )
        approxPolyDP(Mat(contours0[k]), contours[k], 3, true);

    ok = true;
    return ok;
}

void cObjectClassifier::FindBlobs(const cv::Mat &binary, std::vector < std::vector<cv::Point2i> > &blobs, std::vector <cv::Point2d> &blobCenters)
{
    //std::vector <cv::Point2d> blobCenters;

    blobs.clear();

    // Fill the label_image with the blobs
    // 0  - background
    // 1  - unlabelled foreground
    // 2+ - labelled foreground

    cv::Mat label_image;
    binary.convertTo(label_image, CV_32SC1);

    int label_count = 2; // starts at 2 because 0,1 are used already

    for(int y=0; y < label_image.rows; y++)
    {
        int *row = (int*)label_image.ptr(y);
        for(int x=0; x < label_image.cols; x++)
        {
            if(row[x] != 1)
            {
                continue;
            }

            cv::Rect rect;
            //use floodFill to fill-in all neighboring points, returns area and ptr to rect surrounding flood-filled region


            int area = cv::floodFill(label_image, cv::Point(x,y), label_count, &rect, 0, 0, 4);

            std::vector <cv::Point2i> blob;

            double center_x = (double)rect.x + (double)rect.width/2.0;
            double center_y = (double)rect.y + (double)rect.height/2.0;
            cv::Point2d pCenter;
            pCenter.x = center_x;
            pCenter.y = center_y;

            //append to blob_centers std::vector
            blobCenters.push_back(pCenter);


            int cnt = 0;

            for(int i=rect.y; i < (rect.y+rect.height); i++)
            {
                int *row2 = (int*)label_image.ptr(i);
                for(int j=rect.x; j < (rect.x+rect.width); j++)
                {
                    if(row2[j] != label_count)
                    {
                        continue;
                    }

                    blob.push_back(cv::Point2i(j,i));

                }
            }

            blobs.push_back(blob);

            label_count++;
        }
    }
}





