#include <QtMath>
#include <QString>

#include "copencv_utils.h"
// OpenCv drawing
// http://docs.opencv.org/2.4/doc/tutorials/core/basic_geometric_drawing/basic_geometric_drawing.html

cOpenCv_Utils::cOpenCv_Utils(QObject *parent) : QObject(parent)
{

}

QString cOpenCv_Utils::convert_point_to_csv_QString(Point pt)
{
    QString sx = QString::number(pt.x);
    QString sy = QString::number(pt.y);

    QString sPt = sx + "," + sy;

    return sPt;
}

QString cOpenCv_Utils::convert_point3i_to_csv_QString(Point3i pt)
{
    QString sx = QString::number(pt.x);
    QString sy = QString::number(pt.y);
    QString sz = QString::number(pt.z);

    QString sPt = sx + "," + sy + "," + sz;

    return sPt;
}


//use to form a randomly ordered list; preserves row integrity
cv::Mat cOpenCv_Utils::shuffleRows(const cv::Mat &matrix)
{
  std::vector <int> seeds;

  //create a list of index's, for each element in Mat matrix
  for (int cont = 0; cont < matrix.rows; cont++)
    seeds.push_back(cont);

  //randomly re-order index list
  cv::randShuffle(seeds);

  //create output Matrix using randomly re-ordered index list
  cv::Mat output;

  for (int cont = 0; cont < matrix.rows; cont++)
    output.push_back(matrix.row(seeds[cont]));


  return output;
}

//modify input image to reflect current zoom and pan settings
//output will be a 640x480 image
void cOpenCv_Utils::process_zoomPan(Mat &img, int zoom_factor, Point pan_topLeft, Point output_image_size)
{
  Mat imageROI;

    int DISPLAY_PIXEL_WIDTH = output_image_size.x;
    int DISPLAY_PIXEL_HEIGHT = output_image_size.y;



    //Point pan_center, pan_topLeft;
    //int zoom_factor=1;
    //bool isZoomed=false;
    //bool use_panCenter = false;

    if(zoom_factor==1)
    {
        return;  //nothing to do
    }
    else
    {
        //resize image
        int nrows = img.rows;
        int ncols = img.cols;

        nrows = nrows * zoom_factor;
        ncols = ncols * zoom_factor;

        //image resize for zoom
        cv::resize(img,imageROI,cv::Size(ncols,nrows));

        //now set roi
        int left, right, top, bottom;
        left = pan_topLeft.x;
        right = pan_topLeft.x+DISPLAY_PIXEL_WIDTH;
        top = pan_topLeft.y;
        bottom = pan_topLeft.y+DISPLAY_PIXEL_HEIGHT;


        int max_left = ncols - DISPLAY_PIXEL_WIDTH-1;
        if(left > max_left)
        {
            left = max_left;
        }
        int max_top = nrows - DISPLAY_PIXEL_HEIGHT-1;
        if(top > max_top)
        {
            top = max_top;
        }


        cv::Rect rect = cv::Rect(left,top,DISPLAY_PIXEL_WIDTH,DISPLAY_PIXEL_HEIGHT);

        nrows = rect.width;
        ncols = rect.height;

        //Mat imageROI = Mat::zeros(nrows, ncols, CV_8UC3);

        //get new image based on roi rect for pan setting
        img= imageROI(rect);

    }

}




//extract a subset image, spec'd by roi
Mat cOpenCv_Utils::get_Mat_from_imageROI(Mat img, Rect roi)
{
    cv::Mat imageROI;
    //imageROI= img(cv::Rect(385,270,logo.cols,logo.rows));
    imageROI= img(roi);

    return imageROI;
}

Mat cOpenCv_Utils::set_image_roi(Mat img, Rect region_of_interest)
{
    //Rect region_of_interest = Rect(x, y, w, h);
    Mat image_roi = img(region_of_interest);

    return image_roi;
}

Mat cOpenCv_Utils::norm_0_255(InputArray _src)
{
    Mat src = _src.getMat();
    // Create and return normalized image:
    Mat dst;
    switch(src.channels())
    {
    case 1:
        cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
        break;
    case 3:
        cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
        break;
    default:
        src.copyTo(dst);
        break;
    }
    return dst;
}

/*
void cOpenCv_Utils::read_csv(const string *filename, vector<Mat> *images, vector<int> *labels, char separator = ';')
{
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file)
    {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line))
    {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty())
        {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}
*/


//Companion to readMatFromFile()
//writes double precision Mat to file; for later reading with readMatFromFile()
// from: http://beansandbits.blogspot.com/2012/07/readwrite-floating-point-images-with.html
int cOpenCv_Utils::writeMatToFile(const Mat &I, string path)
{

    //load the matrix size
    int matWidth = I.size().width, matHeight = I.size().height;

    //read type from Mat
    int type = I.type();

    //declare values to be written
    float fvalue;
    double dvalue;
    Vec3f vfvalue;
    Vec3d vdvalue;

    //create the file stream
    ofstream file(path.c_str(), ios::out | ios::binary );
    if (!file)
        return -1;

    //write type and size of the matrix first
    file.write((const char*) &type, sizeof(type));
    file.write((const char*) &matWidth, sizeof(matWidth));
    file.write((const char*) &matHeight, sizeof(matHeight));

    //write data depending on the image's type
    switch (type)
    {
    default:
        cout << "Error: wrong Mat type: must be CV_32F, CV_64F, CV_32FC3 or CV_64FC3" << endl;
        break;
    // FLOAT ONE CHANNEL
    case CV_32F:
        cout << "Writing CV_32F image" << endl;
        for (int i=0; i < matWidth*matHeight; ++i) {
            fvalue = I.at<float>(i);
            file.write((const char*) &fvalue, sizeof(fvalue));
        }
        break;
    // DOUBLE ONE CHANNEL
    case CV_64F:
        cout << "Writing CV_64F image" << endl;
        for (int i=0; i < matWidth*matHeight; ++i) {
            dvalue = I.at<double>(i);
            file.write((const char*) &dvalue, sizeof(dvalue));
        }
        break;

    // FLOAT THREE CHANNELS
    case CV_32FC3:
        cout << "Writing CV_32FC3 image" << endl;
        for (int i=0; i < matWidth*matHeight; ++i) {
            vfvalue = I.at<Vec3f>(i);
            file.write((const char*) &vfvalue, sizeof(vfvalue));
        }
        break;

    // DOUBLE THREE CHANNELS
    case CV_64FC3:
        cout << "Writing CV_64FC3 image" << endl;
        for (int i=0; i < matWidth*matHeight; ++i) {
            vdvalue = I.at<Vec3d>(i);
            file.write((const char*) &vdvalue, sizeof(vdvalue));
        }
        break;

    }

    //close file
    file.close();

    return 0;
}

//Companion to writeMatToFile()
//read double precision Mat from file save with writeMatToFile()
int cOpenCv_Utils::readFileToMat(Mat &I, string path)
{

    //declare image parameters
    int matWidth, matHeight, type;

    //declare values to be written
    float fvalue;
    double dvalue;
    Vec3f vfvalue;
    Vec3d vdvalue;

    //create the file stream
    ifstream file(path.c_str(), ios::in | ios::binary );
    if (!file)
        return -1;

    //read type and size of the matrix first
    file.read((char*) &type, sizeof(type));
    file.read((char*) &matWidth, sizeof(matWidth));
    file.read((char*) &matHeight, sizeof(matHeight));

    //change Mat type
    I = Mat::zeros(matHeight, matWidth, type);

    //write data depending on the image's type
    switch (type)
    {
    default:
        cout << "Error: wrong Mat type: must be CV_32F, CV_64F, CV_32FC3 or CV_64FC3" << endl;
        break;
    // FLOAT ONE CHANNEL
    case CV_32F:
        cout << "Reading CV_32F image" << endl;
        for (int i=0; i < matWidth*matHeight; ++i) {
            file.read((char*) &fvalue, sizeof(fvalue));
            I.at<float>(i) = fvalue;
        }
        break;
    // DOUBLE ONE CHANNEL
    case CV_64F:
        cout << "Reading CV_64F image" << endl;
        for (int i=0; i < matWidth*matHeight; ++i) {
            file.read((char*) &dvalue, sizeof(dvalue));
            I.at<double>(i) = dvalue;
        }
        break;

    // FLOAT THREE CHANNELS
    case CV_32FC3:
        cout << "Reading CV_32FC3 image" << endl;
        for (int i=0; i < matWidth*matHeight; ++i) {
            file.read((char*) &vfvalue, sizeof(vfvalue));
            I.at<Vec3f>(i) = vfvalue;
        }
        break;

    // DOUBLE THREE CHANNELS
    case CV_64FC3:
        cout << "Reading CV_64FC3 image" << endl;
        for (int i=0; i < matWidth*matHeight; ++i) {
            file.read((char*) &vdvalue, sizeof(vdvalue));
            I.at<Vec3d>(i) = vdvalue;
        }
        break;

    }

    //close file
    file.close();

    return 0;
}

//assumes 3 channel color image; will return if it's not
//if not; use createMask_from_poly
void cOpenCv_Utils::paint_poly_to_image(Mat src, Mat *dest, vector<Point> mask_pts, cv::Scalar clr)
{
    //get pixel count of our class
    int nRows, nCols;
    nRows = src.rows;
    nCols = src.cols;

    int nchan = src.channels();

    if(nchan != 3)
    {
        return;
    }


     //create mask image where only pixels that aren't zero are inside polygon provided by set of mask_pts
     Mat mask_img = src.clone();



     //draw filled poly using mask_pts
     int N = static_cast<int>(mask_pts.size());
     Point poly_pts[1][N];

     for(int i=0;i<N;i++)
     {
         poly_pts[0][i] = mask_pts[i];
     }

     const Point* ppt[1] = { poly_pts[0] };
     int npt[] = {N};

     int lineType = LINE_8;

     fillPoly(mask_img, ppt, npt, 1, Scalar(255),lineType);

     *dest = mask_img;

     //imshow("Polygon Painted Image",mask_img);
     //imshow("Polygon Mask Image Copy",*dest);

}


//tested works
void cOpenCv_Utils::createMask_from_poly(Mat img, Mat *img_mask, vector<Point> mask_pts)
{
    //get pixel count of our class
    int nRows, nCols;
    nRows = img.rows;
    nCols = img.cols;

//    imshow("test image in CreateMask_from_poly",img);
// return;

     //create mask image where only pixels that aren't zero are inside polygon provided by set of mask_pts
     Mat mask_img = Mat::zeros(nRows, nCols,CV_8U);


     //draw filled poly using mask_pts
     int N = static_cast<int>(mask_pts.size());
     Point poly_pts[1][N];

     for(int i=0;i<N;i++)
     {
         poly_pts[0][i] = mask_pts[i];
     }

     const Point* ppt[1] = { poly_pts[0] };
     int npt[] = {N};

     int lineType = LINE_8;

     fillPoly(mask_img, ppt, npt, 1, Scalar(255),lineType);

     *img_mask = mask_img;

     //imshow("Polygon Mask Image",mask_img);
    // imshow("Polygon Mask Image Copy",*img_mask);

}



void cOpenCv_Utils::drawPoint(Mat *img, Point center, cv::Scalar color)
{
    int radius = 1;
    int thickness = 2;
    int lineType = 8;

    circle(*img, center, radius, color,thickness, lineType);

    /*  Point usage
     * Point pt =  Point(10, 8);
     * pt.x = 10;
     * pt.y = 20;
     *
     * cv::Scalar myWhite(255,255,255)
     * cout << myWhite[1] << endl;
     */

}

double cOpenCv_Utils::get_dist_2Points(Point pt1, Point pt2)
{
    double x1 = (double)pt1.x;
    double y1 = (double)pt1.y;
    double x2 = (double)pt2.x;
    double y2 = (double)pt2.y;

    double distsqrd = (x1-x2)*(x1-x1) + (y1-y2)*(y1-y1);
    double dist = qSqrt(distsqrd);

    return dist;
}


void cOpenCv_Utils::drawLine(Mat img, Point start, Point end, cv::Scalar color)
{
    int thickness = 1;
    int lineType = 8;
     line( img,
           start,
           end,
           color,  //BGR
           thickness,
           lineType );
}

//for example on setting up pp_pts array see
// http://docs.opencv.org/3.2.0/d3/d96/tutorial_basic_geometric_drawing.html
void cOpenCv_Utils::drawFilledPoly(Mat img, const Point **pp_pts, int npts, cv::Scalar color)
{
    int lineType = LINE_8; //connected on 8 sides
    int npt[] = { npts };


    fillPoly( img,
                pp_pts,  //CvPoint** pts
                npt,   //int* npts
                1,  //# of polys
                color,   //BGR
                lineType );

}


void cOpenCv_Utils::drawGreenPoint(Mat img, Point center)
{
    cv::Scalar color(0,255,0);

    drawPoint(&img,center,color);
}

void cOpenCv_Utils::drawRedPoint(Mat img, Point center)
{
    cv::Scalar color(0, 0, 255);

    drawPoint(&img,center,color);
}

void cOpenCv_Utils::drawRedLine(Mat img, Point start, Point end)
{
    cv::Scalar color(0,0,255);

    drawLine(img,start, end, color);
}

void cOpenCv_Utils::drawGreenLine(Mat img, Point start, Point end)
{
    cv::Scalar color(0,255,0);

    drawLine(img,start, end, color);
}

void cOpenCv_Utils::drawRedFilledPoly(Mat img, const Point **pp_pts, int npts)
{
    int lineType = 8; //connected on 8 sides

    cv::Scalar color(0,0,255); //BGR

    drawFilledPoly(img, pp_pts, npts, color);
}

void cOpenCv_Utils::drawGreenFilledPoly(Mat img, const Point **pp_pts, int npts)
{
    int lineType = 8; //connected on 8 sides

    cv::Scalar color(0,255,0); //BGR

    drawFilledPoly(img, pp_pts, npts, color);
}


//binned plot
void cOpenCv_Utils::plot_hs_histogram(Mat src, Mat mask )
{
    Mat hsv;

    cvtColor(src, hsv, CV_BGR2HSV);

    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
    int hbins = 30, sbins = 32;
    int histSize[] = {hbins, sbins};
    // hue varies from 0 to 179, see cvtColor
    float hranges[] = { 0, 180 };
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
    float sranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges };
    MatND hist;
    // we compute the histogram from the 0-th and 1-st channels
    int channels[] = {0, 1};

    calcHist( &hsv, 1, channels, Mat(), // do not use mask
             hist, 2, histSize, ranges,
             true, // the histogram is uniform
             false );
    double maxVal=0;
    minMaxLoc(hist, 0, &maxVal, 0, 0);

    int scale = 10;
    Mat histImg = Mat::zeros(sbins*scale, hbins*10, CV_8UC3);

    for( int h = 0; h < hbins; h++ )
        for( int s = 0; s < sbins; s++ )
        {
            float binVal = hist.at<float>(h, s);
            int intensity = cvRound(binVal*255/maxVal);
            rectangle( histImg, Point(h*scale, s*scale),
                        Point( (h+1)*scale - 1, (s+1)*scale - 1),
                        Scalar::all(intensity),
                        CV_FILLED );
        }

    //namedWindow( "Source", 1 );
    //imshow( "Source", src );

    namedWindow( "H-S Histogram", 1 );
    imshow( "H-S Histogram", histImg );
}

void cOpenCv_Utils::plot_hsv_histogram( Mat img, Mat mask  )
{
  //Mat src, dst;
  Mat hsv_img;

  cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);

  if( !hsv_img.data )
    { return ; }

  /// Separate the image in 3 places ( B, G and R )
  vector<Mat> hsv_planes;
  split( hsv_img, hsv_planes );

  cv::Scalar hsv_clr = cv::mean(hsv_img,mask);

  float h = hsv_clr[0];
  float s = hsv_clr[1];
  float v = hsv_clr[2];


  /// Establish the number of bins
  int histSize = 256;

  /// Set the ranges ( for H,S,V) )
  float h_range[] = { 0, 255 } ;
  float sv_range[] = { 0, 255 } ;

  const float* h_histRange = { h_range };
  const float* sv_histRange = { sv_range };

  bool uniform = true; bool accumulate = false;

  Mat h_hist, s_hist, v_hist;

  /// Compute the histograms:
  calcHist( &hsv_planes[0], 1, 0, mask, h_hist, 1, &histSize, &h_histRange, uniform, accumulate );
  calcHist( &hsv_planes[1], 1, 0, mask, s_hist, 1, &histSize, &sv_histRange, uniform, accumulate );
  calcHist( &hsv_planes[2], 1, 0, mask, v_hist, 1, &histSize, &sv_histRange, uniform, accumulate );

  // Draw the histograms for B, G and R
  int hist_w = 512; int hist_h = 400;
  int bin_w = cvRound( (double) hist_w/histSize );

  Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

  /// Normalize the result to [ 0, histImage.rows ]
  normalize(h_hist, h_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  normalize(s_hist, s_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  normalize(v_hist, v_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );


  /// Draw for each channel
  for( int i = 0; i < histSize; i++ )
  {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(h_hist.at<float>(i-1)) ) ,
                        Point( bin_w*(i), hist_h - cvRound(h_hist.at<float>(i)) ),
                        Scalar( 255, 0, 0), 2, 8, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(s_hist.at<float>(i-1)) ) ,
                        Point( bin_w*(i), hist_h - cvRound(s_hist.at<float>(i)) ),
                        Scalar( 0, 255, 0), 2, 8, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(v_hist.at<float>(i-1)) ) ,
                        Point( bin_w*(i), hist_h - cvRound(v_hist.at<float>(i)) ),
                        Scalar( 0, 0, 255), 2, 8, 0  );
  }

  /// Display
  namedWindow("HSV Histogram: 'H:Blue, S:Green, V:Red'", WINDOW_AUTOSIZE );
  imshow("HSV Histogram: 'H:Blue, S:Green, V:Red'", histImage );


  return ;
}

void cOpenCv_Utils::set_color(Mat &img, int row, int col, Vec3b clr)
{
    int nrows = img.rows;
    int ncols = img.cols;
    int nchans = 3;

    uchar* pixelPtr = img.data;

    uchar R,G,B;
    B = clr[0];
    G = clr[1];
    R = clr[2];

    const int pi = row*ncols*3 + col*nchans;

    pixelPtr[pi + 0] = B; // B
    pixelPtr[pi + 1] = G; // G
    pixelPtr[pi + 2] = R; // R
}

Vec3b cOpenCv_Utils::get_color(Mat img, int row, int col)
{
    Vec3b clr;

    int nrows = img.rows;
    int ncols = img.cols;
    int nchans = 3;

    uchar* pixelPtr = img.data;

    uchar R,G,B;

    const int pi = row*ncols*3 + col*nchans;

    B = pixelPtr[pi + 0]; // B
    G = pixelPtr[pi + 1]; // G
    R = pixelPtr[pi + 2]; // R

    clr[0] = B;
    clr[1] = G;
    clr[2] = R;

    return clr;
}

Vec3b cOpenCv_Utils::get_color_hsv(Mat img, int row, int col)
{
    Mat hsv_mini_img;
    Vec3b hsv_clr;

    //set ROI
    Rect rect(row,col,1,1);  //rect(startRow,startCol, width, height)

    Mat mini_img = img(rect);

    cvtColor(mini_img, hsv_mini_img, cv::COLOR_BGR2HSV);

    Mat first_row(hsv_mini_img.row(0));

    vector<Vec3b> clr_vect(first_row.begin<Vec3b>(),first_row.end<Vec3b>());

    Vec3b clr = cOpenCv_Utils::get_color(img, row, col);

    Mat clrMat = cv::Mat(clr);

    int nchans = clrMat.channels();

    //clrMat.convertTo(tClrMat, CV_8UC3);

    //cvtColor(tClrMat, hsvClrMat, cv::COLOR_BGR2HSV);


    //hsv_clr[0] = tClrMat.at<Vec3b>(0,0)[0];
    //hsv_clr[1] = tClrMat.at<Vec3b>(0,0)[1];
    //hsv_clr[2] = tClrMat.at<Vec3b>(0,0)[2];

    return hsv_clr;
}

vector<Vec3b> cOpenCv_Utils::convert_Mat_to_vector_vec3b(Mat img)
{
    vector<Vec3b> clr_vect;

    int nRows = img.rows;

    for(int i=0;i<nRows;i++)
    {
        vector<Vec3b> line_vect(img.row(i).begin<Vec3b>(),img.row(i).end<Vec3b>());

//        clr_vect.push_back(line_vect);
    }


    return clr_vect;
}

vector<Vec3b> cOpenCv_Utils::convert_Mat_to_vector_vec3b(Mat img, Rect rect)
{
    Mat mini_img = img(rect);

    vector<Vec3b> v_array = convert_Mat_to_vector_vec3b(img);

    return v_array;
}

vector<Vec3b> cOpenCv_Utils::convert_Mat_to_vector_vec3b(Mat img, int row, int col, int width, int height)
{
    Rect rect(row,col,width,height);  //rect(startRow,startCol, width, height)

    vector<Vec3b> v_array = convert_Mat_to_vector_vec3b(img, rect);

    return v_array;

}

bool cOpenCv_Utils::copy_srcMatRect_to_dstMatRect(Mat miniMat_src, Mat &dstMat, Rect srcRect, Rect dstRect)
{
    bool ok = false;

    //ck to ensure src and dst Mat's are same type
    if (miniMat_src.type() != dstMat.type())
        return ok;

    //ck srcRect is same size as dstRect

    //ck srcRect is inside src image

    //ck dstRect is inside dst image


    //copy the src Rect to dstMat's Rect
    miniMat_src(srcRect).copyTo(dstMat(dstRect));



}

Mat cOpenCv_Utils::create_color_mat(Rect imgRect, uchar blueVal, uchar greenVal, uchar redVal)
{
    Vec3b clr(blueVal,greenVal,redVal);

    Mat img = create_color_mat(imgRect, clr);

    return img;
}

Mat cOpenCv_Utils::create_color_mat(int width, int height, uchar blueVal, uchar greenVal, uchar redVal)
{
    Rect imgRect(0,0,width,height);
    Vec3b clr(blueVal,greenVal,redVal);

    Mat img = create_color_mat(imgRect, clr);

    return img;
}

Mat cOpenCv_Utils::create_color_mat(int width, int height, Vec3b clr)
{
    Rect imgRect(0,0,width,height);
    Mat img = create_color_mat(imgRect, clr);

    return img;
}

Mat cOpenCv_Utils::create_color_mat(Rect imgRect, Vec3b clr)
{

    uchar R,G,B;
    B = clr[0];
    G = clr[1];
    R = clr[2];

    Mat img(imgRect.height,imgRect.width,CV_8UC3,Scalar(B,G,R));


    return img;
}

void cOpenCv_Utils::saveImage(Mat img)
{
    cUtils cu;

    QString filePathName = cu.get_saveFileName();

    std::string sstrFilePathName = cu.convert_QString_to_stdString(filePathName);

    imwrite(sstrFilePathName,img);


}



//sort function for knn_getBestSelection
bool sort_knnBest (const cv::Point p1, const cv::Point p2)
{
    //return( (p1.x + p1.y) < (p2.x + p2.y) );
    return( p1.y >  p2.y );
}

int cOpenCv_Utils::knn_getBestSelection(Mat neighborsClassList, int MAX_NUM_CLASSES)
{
    vector<Point> classCounts( (MAX_NUM_CLASSES-1), Point(0.0,0.0));  //INIT vector to zeros

    int nrows = neighborsClassList.rows;
    int ncols = neighborsClassList.cols;

    int n = ncols;

    for(int i=0;i<n;i++)
    {
        float resp = neighborsClassList.at<float>(0,i);

        int iResp = (int)resp;

        //get indx rep. selected class choice
        int indx = iResp;
        //add 1 to each class
        classCounts[indx].x = indx;  //store class# (indx) in x (so we'll know what it is when it's sorted later
        classCounts[indx].y ++;
    }

    //NOW SORT (note the function ptr to comparison function "sort_knnBest()"
    std::sort(classCounts.begin(),classCounts.end(), sort_knnBest);

    //best class should now be in position 0
    int bestClass = classCounts[0].x;  // x holds class id, y holds voting count

    return bestClass;
}

