#include <QMessageBox>

#include "cclassifier.h"


/*
 * Each class training set should be saved separately, so that the master training set can be easily  rebuilt, should the use decide to
 * over-write the class training data
 *
 * Build ALL Styles of Training Classifiers (traditional variants; svm, nn, knn, bayes); so that if user wants to switch types of classifier,
 * he can do so easily and quickly.
 *
 * Be great to report which is the most accurate and to list accuracy of each version;  for this we need a function to split training set randomly
 * so split Master Training set into-> {part_A_master_trainingSet, part_B_master_trainingSet}.
 * In practice; train on Part-A and verify accuracy on Part-B
 *
 * that's going to take some work.... how to randomly re-arrange a Mat?? while preserving pairs?
 *
 */

cClassifier::cClassifier(QObject *parent) : QObject(parent)
{
    cocv = new cOpenCv_Utils();

    clear_classifier();
}

/*
void cClassifier::use_classifier(Mat &img)
{
    if(classifier_ready_to_use)
    {
        //select which classifier to use

        if(use_LUT_to_classify_image)
        {
            if(use_three_colors)
            {
                process_image_3dLut(img);
            }
            else
            {
                process_image_lut(img);
            }
        }

    }
    else
    {
        //signal error; classifier not available; needs to be built or loaded

        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Error: classifier not available, needs to be built or loaded");
        return;
    }
}
*/

void cClassifier::display_lut()
{


}

bool cClassifier::build_lut()
{
    bool ok = true;

    int x = 0;
    switch(classifier_type_to_use)
    {
        case 1:
            if(use_three_colors)
            {
                build_3dlut_bayes();
            }
            else
            {
                build_lut_bayes();
            }
            break;
        case 2:
            build_lut_knn();
            break;
        case 3:
            build_lut_svm();
            break;
        default:
            build_lut_bayes();
    }

    return ok;
}

void cClassifier::build_lut_svm()
{

    if(classifier_ready_to_use)
    {
        hs_Lut.release();

        //use classifier to build LUT
        hs_Lut = Mat::zeros(256,256,CV_8U);
        Mat img_Lut = Mat::zeros(256,256,CV_8UC3);

        //Paint out to image regions as classed color for location (training data is 2d vector from 0:512)

        Vec3b green(0,255,0), blue (255,0,0), red(0,0,255), yellow(0,255,255);
        Vec3b cyan(255,255,0), white(255,255,255), black(0,0,0), brown(80,90,140), grey(90,90,90);

        // Show the decision regions given by the SVM
        for (int y = 0; y < 256; ++y)
        {
            for (int x = 0; x < 256; ++x)
            {
                Mat sampleMat = (Mat_<float>(1,2) << y,x);

                //Mat output, outputProb;
                //ptr_bayesCl->predictProb(sampleMat,output, outputProb);

                float resp = ptr_svm->predict(sampleMat);

                int response = (int)(resp);

                hs_Lut.at<unsigned char>(y,x) = (unsigned char)response;

                int r = (int)hs_Lut.at<unsigned char>(y,x);

                switch(r)
                {
                case 0:
                    img_Lut.at<Vec3b>(y,x) = white;
                    break;
                case 1:
                    img_Lut.at<Vec3b>(y,x) = yellow;
                    break;
                case 2:
                    img_Lut.at<Vec3b>(y,x) = blue;
                    break;
                case 3:
                    img_Lut.at<Vec3b>(y,x) = green;
                    break;
                case 4:
                    img_Lut.at<Vec3b>(y,x) = red;
                    break;
                case 5:
                    img_Lut.at<Vec3b>(y,x) = brown;
                    break;
                case 6:
                    img_Lut.at<Vec3b>(y,x) = cyan;
                        break;
                case 7:
                    img_Lut.at<Vec3b>(y,x) = black;
                    break;
                default:
                    img_Lut.at<Vec3b>(y,x) = grey;
                    break;
                }

            }
        }

        Mat lut_img = img_Lut.clone();

        int nchans = lut_img.channels();
        int nrows = lut_img.rows;
        int ncols = lut_img.cols;

        //display image of classifier (use different colors for each class)
        imshow("HS Lut",img_Lut);
    }
    else
    {
        //signal error; classifier not available; needs to be built or loaded

        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Error: classifier not available, needs to be built or loaded first, before you can build LUT");
        return;
    }

}

void cClassifier::build_lut_knn()
{

    if(classifier_ready_to_use)
    {
        hs_Lut.release();

        //use classifier to build LUT
        hs_Lut = Mat::zeros(256,256,CV_8U);
        Mat img_Lut = Mat::zeros(256,256,CV_8UC3);

        //Paint out to image regions as classed color for location (training data is 2d vector from 0:512)

        Vec3b green(0,255,0), blue (255,0,0), red(0,0,255), yellow(0,255,255);
        Vec3b cyan(255,255,0), white(255,255,255), black(0,0,0), brown(80,90,140), grey(90,90,90);

        const int MAX_NUM_CLASSES = 6;
        int K=1, accuracy;  //#of nearest neighbors to use
        Mat nearestNeighbors, distances;
        float faSample[2];
        Mat sampleMat(1,2,CV_32FC1,faSample);

        // Show the decision regions given by the SVM
        for (int y = 0; y < 256; ++y)
        {
            for (int x = 0; x < 256; ++x)
            {
                Mat sampleMat = (Mat_<float>(1,2) << y,x);

                //returns list of closest K nearest neighbors;  have to figure which is most likely
                ptr_knn->findNearest(sampleMat,K,noArray(),nearestNeighbors,distances);


                //const cv::Mat sample = nearestNeighbors.row(0);
                //float h = sample.at<cv::Vec2f>(0)[0];
                //float s = sample.at<cv::Vec2f>(0)[1];
                //int response = cocv->knn_getBestSelection(nearestNeighbors, MAX_NUM_CLASSES);
                int response = (int)nearestNeighbors.at<float>(0,0);

                hs_Lut.at<unsigned char>(y,x) = (unsigned char)response;

                int r = (int)hs_Lut.at<unsigned char>(y,x);

                switch(r)
                {
                    case 0:
                        img_Lut.at<Vec3b>(y,x) = white;
                        break;
                    case 1:
                        img_Lut.at<Vec3b>(y,x) = yellow;
                        break;
                    case 2:
                        img_Lut.at<Vec3b>(y,x) = blue;
                        break;
                    case 3:
                        img_Lut.at<Vec3b>(y,x) = green;
                        break;
                    case 4:
                        img_Lut.at<Vec3b>(y,x) = red;
                        break;
                    case 5:
                        img_Lut.at<Vec3b>(y,x) = brown;
                        break;
                    case 6:
                        img_Lut.at<Vec3b>(y,x) = cyan;
                            break;
                    case 7:
                        img_Lut.at<Vec3b>(y,x) = black;
                        break;
                    default:
                        img_Lut.at<Vec3b>(y,x) = grey;
                        break;
                }

            }
        }

        Mat lut_img = img_Lut.clone();

        int nchans = lut_img.channels();
        int nrows = lut_img.rows;
        int ncols = lut_img.cols;

        //display image of classifier (use different colors for each class)
        imshow("HS Lut",img_Lut);
    }
    else
    {
        //signal error; classifier not available; needs to be built or loaded

        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Error: classifier not available, needs to be built or loaded first, before you can build LUT");
        return;
    }


}

void cClassifier::build_lut_bayes()
{
    if(classifier_ready_to_use)
    {
        hs_Lut.release();

        //use classifier to build LUT
        hs_Lut = Mat::zeros(256,256,CV_8U);
        Mat img_Lut = Mat::zeros(256,256,CV_8UC3);

        //Paint out to image regions as classed color for location (training data is 2d vector from 0:512)

        Vec3b green(0,255,0), blue (255,0,0), red(0,0,255), yellow(0,255,255);
        Vec3b cyan(255,255,0), white(255,255,255), black(0,0,0), brown(80,90,140), grey(90,90,90);

        // Show the decision regions given by the SVM
        for (int y = 0; y < 256; ++y)
        {
            for (int x = 0; x < 256; ++x)
            {
                Mat sampleMat = (Mat_<float>(1,2) << y,x);

                Mat output, outputProb;
                ptr_bayesCl->predictProb(sampleMat,output, outputProb);

                int response = output.at<int>(0,0);

                hs_Lut.at<unsigned char>(y,x) = (unsigned char)response;

                int r = (int)hs_Lut.at<unsigned char>(y,x);

                switch(r)
                {
                case 0:
                    img_Lut.at<Vec3b>(y,x) = white;
                    break;
                case 1:
                    img_Lut.at<Vec3b>(y,x) = yellow;
                    break;
                case 2:
                    img_Lut.at<Vec3b>(y,x) = blue;
                    break;
                case 3:
                    img_Lut.at<Vec3b>(y,x) = green;
                    break;
                case 4:
                    img_Lut.at<Vec3b>(y,x) = red;
                    break;
                case 5:
                    img_Lut.at<Vec3b>(y,x) = brown;
                    break;
                case 6:
                    img_Lut.at<Vec3b>(y,x) = cyan;
                        break;
                case 7:
                    img_Lut.at<Vec3b>(y,x) = black;
                    break;
                default:
                    img_Lut.at<Vec3b>(y,x) = grey;
                    break;
                }

            }
        }

        Mat lut_img = img_Lut.clone();

        int nchans = lut_img.channels();
        int nrows = lut_img.rows;
        int ncols = lut_img.cols;

        //display image of classifier (use different colors for each class)
        imshow("HS Lut",img_Lut);
    }
    else
    {
        //signal error; classifier not available; needs to be built or loaded

        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Error: classifier not available, needs to be built or loaded first, before you can build LUT");
        return;
    }

}

void cClassifier::build_3dlut_bayes()
{
    const int SATURATION_THRESHOLD_RED = 105;  //if lower than this amount; class as cotton
    const int SATURATION_THRESHOLD_YELLOW = 65;  //if lower than this amount; class as cotton
    int class0_cnt = 0;
    int class1_cnt = 0;

    if(classifier_ready_to_use)
    {
        hs_3dLut[256][256][256];

        for (int H = 0; H < 256; ++H)
        {
            for (int S = 0; S < 256; ++S)
            {
                for (int V = 0; V < 256; ++V)
                {
                    Mat sampleMat = (Mat_<float>(1,3) << H,S,V);

                    Mat output, outputProb;
                    ptr_bayesCl->predictProb(sampleMat,output, outputProb);

                    int response = output.at<int>(0,0);

                    if(S<SATURATION_THRESHOLD_YELLOW)
                    {
                        response = 0;
                    }
                    else if(S<SATURATION_THRESHOLD_RED)
                    {
                        if(response!=1)  //skip if it's yellow
                        {
                            response = 0;
                        }
                    }

                    if(response == 0)
                    {
                        class0_cnt++;
                    }
                    else
                    {
                        class1_cnt++;
                    }

                    hs_3dLut[H][S][V]  = (unsigned char)response;

                }
            }
        }


    }
    else
    {
        //signal error; classifier not available; needs to be built or loaded

        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Error: classifier not available, needs to be built or loaded first, before you can build LUT");
        return;
    }

    hs_3dLut[0][0][0] = (unsigned char)99;

    int cl_cnt = class0_cnt;
}

void cClassifier::process_image(Mat &img)
{

    if(use_three_colors)
    {
        process_image_3dLut(img);
    }
    else
    {
        process_image_lut(img);
    }
}

void cClassifier::process_image_3dLut(Mat &img)
{
    //assume valid lut
    int nRows, nCols;
    nRows = img.rows;
    nCols = img.cols;
    int nchans = img.channels();

    if(nchans != 3)
    {
        return;
    }

    //create copy of img (that'll preserve all pixels we don't want to classify)
    Mat classed_image, hsv_img;

    classed_image = img.clone();

    //convert img to hsv space
    cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);

    Vec3b clr_class0(255,255,255); //white
    Vec3b clr_class1(0,255,255);   //yellow
    Vec3b clr_class2(255,0,0);     //blue
    Vec3b clr_class3(0,255,0);     //green
    Vec3b clr_class4(23,24,58);     //low-red
    Vec3b clr_class5(120,120,180);    //brown
    Vec3b clr_class6(203,195,180);        //shaded-cotton
    Vec3b clr_class7(108,105,206);    //high-red
    Vec3b clr_class8(140,78,67);    //dark-blue
    Vec3b clr_class9(230,255,236);    //light-bur
    Vec3b clr_class10(0,0,0);    //black


    //                                              1-white         2-yellow         3-blue      4-green       5-low_red      6-brown      7-shad.cot    8-high-red     9-dark-blue   10-light-bur
    //            (OCV HSV; so Hue  0-180) HSV = { { 81,  9,100}, { 33, 33, 80}, {112, 60, 99}, { 60,100,100}, { 1,60,23}, {  0, 33, 71}, {101, 11, 80}, {178, 49, 81}, {115,52,55} , { 53, 10,100}};
    //            (Gimp HSV; so Hue 0-359) HSV = { {162,  9,100}, { 67, 33, 80}, {225, 60, 99}, {120,100,100}, { 2,60,23}, {  0, 33, 71}, {201, 11, 80}, {358, 49, 81}, {231,52,55} , {106, 10,100}};
    //float trainingData[nsamples][nclr_chans] = { {248,255,231}, {137,203,195}, {252,138,100}, {  0,255,0  }, {23,24,58}, {120,120,180}, {203,195,180}, {108,105,206}, {140,78,67} , {230,255,236}};


    Vec3b pixel_clr;


    unsigned char response = hs_3dLut[0][0][0];

    unsigned int hot_pixel_count = 0;

    for(int y=0;y<nRows;y++)
    {
        for(int x=0;x<nCols;x++)
        {
            pixel_clr = hsv_img.at<Vec3b>(y,x);
            unsigned int h = pixel_clr[0];
            unsigned int s = pixel_clr[1];
            unsigned int v = pixel_clr[2];

            response = hs_3dLut[h][s][v];

            switch(response)
            {
                case 0:
                    classed_image.at<Vec3b>(y,x) = clr_class0;  //cotton
                    break;
                case 1:
                    classed_image.at<Vec3b>(y,x) = clr_class1;
                    break;
                case 2:
                    classed_image.at<Vec3b>(y,x) = clr_class2;
                    break;
                case 3:
                    classed_image.at<Vec3b>(y,x) = clr_class3;
                    break;
                case 4:
                    classed_image.at<Vec3b>(y,x) = clr_class4;
                    break;
                case 5:
                    //classed_image.at<Vec3b>(y,x) = clr_class5;  //sticks
                    break;
                case 6:
                    //classed_image.at<Vec3b>(y,x) = clr_class6;  //shaded cotton
                    break;
                case 7:
                    classed_image.at<Vec3b>(y,x) = clr_class7;
                    break;
                case 8:
                    classed_image.at<Vec3b>(y,x) = clr_class8;
                    break;
                case 9:
                    classed_image.at<Vec3b>(y,x) = clr_class9;  //burs
                    break;
                default:
                    classed_image.at<Vec3b>(y,x) = clr_class10;
            }

        }
    }

    img = classed_image;

   // imshow("classed image via LUT", classed_image);
}





//assumes hs_Lut is pre-built and stores a 0 for all background pixels (i.e. non-plastic colors)
void cClassifier::process_image_lut(Mat &img)
{
    Mat classed_image, hsv_img;
    int rows = hs_Lut.rows;
    int cols = hs_Lut.cols;

    int x = 2;

    if((rows==256)&&(cols==256))
    {

        //assume valid lut
        int nRows, nCols;
        nRows = img.rows;
        nCols = img.cols;

        //create copy of img (that'll preserve all pixels we don't want to classify)
        classed_image = cv::Mat::zeros(nRows,nCols,CV_8UC3);


        //convert img to hsv space
        cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);

        Vec3b clr_class0(255,255,255);
        Vec3b clr_class1(0,255,255);
        Vec3b clr_class2(255,0,0);
        Vec3b clr_class3(0,255,0);
        Vec3b clr_class4(0,0,255);
        Vec3b clr_class5(80,94,131);
        Vec3b clr_class6(0,0,0);
        Vec3b clr_class7(80,80,80);



        unsigned int h,s,v;
        unsigned int hot_pixel_count = 0;

        for(int y=0;y<nRows;y++)
        {
            for(int x=0;x<nCols;x++)
            {
                Vec3b colour = hsv_img.at<Vec3b>(y,x);  //this format is (row,col)

                h = colour[0];
                s = colour[1];
                v = colour[2];

                unsigned int hs_val;

                if(use_three_colors)
                {
                    hs_val = (unsigned int) hs_Lut.at<unsigned char>(h,s,v);
                }
                else
                {
                    hs_val = (unsigned int) hs_Lut.at<unsigned char>(h,s);
                }


                switch(hs_val)
                {
                    case 0:
                        classed_image.at<Vec3b>(y,x) = clr_class0;
                        break;
                    case 1:
                        classed_image.at<Vec3b>(y,x) = clr_class1;
                        break;
                    case 2:
                        classed_image.at<Vec3b>(y,x) = clr_class2;
                        break;
                    case 3:
                        classed_image.at<Vec3b>(y,x) = clr_class3;
                        break;
                    case 4:
                        classed_image.at<Vec3b>(y,x) = clr_class4;
                        break;
                    case 5:
                        classed_image.at<Vec3b>(y,x) = clr_class5;
                        break;
                    case 6:
                        classed_image.at<Vec3b>(y,x) = clr_class6;
                        break;
                    default:
                        classed_image.at<Vec3b>(y,x) = clr_class7;
                }

            }
        }

        //img = classed_image;

        imshow("classed image via LUT", classed_image);
    }

}


//all done adding regions, so now build it and make it avail
//returns results of build; if successful, and classifier's ready to use, returns true
bool cClassifier::build_classifier()
{
    bool ok = false;

    vector<Point3i> trainData3i, testData3i;
    vector<int> trainLabels3i, testLabels3i;

    ok = master_ccd.get_random_3i_trainingTestSet(trainData3i, trainLabels3i, testData3i, testLabels3i);




    Mat ccd_dataMat, ccd_labelsMat;

    //use entire list
    //master_ccd.get_mat_from_list(ccd_dataMat, ccd_labelsMat);

    //just use training set; so we can post check it with independent test set
    master_ccd.get_mat_from_list(trainData3i, trainLabels3i, ccd_dataMat, ccd_labelsMat);



    // Set up Bayes's parameters
    Ptr<ml::NormalBayesClassifier> bayesCl(ml::NormalBayesClassifier::create());

    ptr_bayesCl = bayesCl;

    //bayesCl->train(ccd_dataMat,ml::SampleTypes::ROW_SAMPLE, ccd_labelsMat);

    ptr_bayesCl->train(ccd_dataMat,ml::SampleTypes::ROW_SAMPLE, ccd_labelsMat);

    isBayesTrained = true;



    Ptr<ml::KNearest> knn(ml::KNearest::create());

    ptr_knn = knn;

    ptr_knn->train(ccd_dataMat,ml::SampleTypes::ROW_SAMPLE,ccd_labelsMat);




    // Set up SVM's parameters
    //works
    Ptr<ml::SVM> svm(ml::SVM::create());
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER,100,1e-6));

    ptr_svm = svm;

    ptr_svm->train(ccd_dataMat,ml::SampleTypes::ROW_SAMPLE,ccd_labelsMat);


    isSvmTrained = true;


    //save model for later
//    bayesCl->save("trainedBayes.xml");

    //now load it from file
//    Ptr<ml::NormalBayesClassifier> bayesCl_new(ml::NormalBayesClassifier::create());
    //bayesCl_new = NormalBayesClassifier::load("trainedBayes.xml");
//    bayesCl_new = NormalBayesClassifier::load<NormalBayesClassifier>("trainedBayes.xml");

    int h=20,s=45,v=10;
    Mat sampleMat;

    if(use_three_colors)
    {
        sampleMat = (Mat_<float>(1,3) << h,s,v);
    }
    else
    {
        sampleMat = (Mat_<float>(1,2) << h,s);
    }

    Mat output, outputProb;
    ptr_bayesCl->predictProb(sampleMat,output, outputProb);
    //float response = output.at<float>(0,0);
    int response = output.at<int>(0,0);


    int N = static_cast<long unsigned int>(testData3i.size());
    int M = static_cast<long unsigned int>(testLabels3i.size());

    Point3i pt;
    int pred_id, orig_id;
    double percent_cor;

    if(N==M)
    {
        int n_cor = 0;

        for(int i=0;i<N;i++)
        {
            orig_id = testLabels3i[i];

            pt = testData3i[i];

            h = pt.x;
            s = pt.y;
            v = pt.z;

            sampleMat = (Mat_<float>(1,3) << h,s,v);

            ptr_bayesCl->predictProb(sampleMat, output, outputProb);

            int response = output.at<int>(0,0);

            if(response == orig_id)
            {
                n_cor++;
            }

            if(response == 0)
            {
                int x = 2;
            }
            else if(response == 1)
            {
                int x = 2;
            }
            else if(response == 2)
            {
                int x = 2;
            }
            else
            {
                int x = 2;
            }

        }

        percent_cor = 100.0*(double)n_cor / (double)N;
    }




    if(isBayesTrained)
    {
        ok = true;

        //if successful; set classifier ready to use
        classifier_ready_to_use = true;
        QMessageBox msgBox;
        QString sPerClassed = QString::number(percent_cor,'f',2);
        QString msg = "Percent correct on Test Set: " + sPerClassed;
        msgBox.critical(0, "Classifier Built", msg);
        return ok;
    }
    else
    {
        ok = true;

        classifier_ready_to_use = true;
        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Classifier did not build successfully; please retry, or debug code inside cclassifier->build_classifier ");
        return ok;
    }

    ok = true;

    return ok;
}



//clear classifier; start over basically, remove any regions already added to trainingSet
void cClassifier::clear_classifier()
{

    master_ccd.clear_all();
}

void cClassifier::add_trainingData_to_dataSet(Mat img, vector<Point> mask_pts, int classId, bool run_histogram, Rect selection_rect)
{
    Mat trainingData, trainingLabels;

    int nRow = img.rows;
    int nCols =img.cols;

//test ok
//imshow("test image in add_trainingData_to_dataSet",img);
//return;

    extractTrainingData(img, mask_pts, classId, selection_rect);

    //now append to our master sets
//    hconcat(master_trainLabels, trainingLabels, master_trainLabels);
//    vconcat(master_trainData,trainingData,master_trainData);



}



//extract training data from subset of image, where subset is provided by a polygon array outlining area to be sampled
//assumes the 1st pt and lastpt of mask_pts are connected (closes array)
void cClassifier::extractTrainingData(Mat img, vector<Point> mask_pts, int classId, Rect selection_rect)
{
   Mat img_mask;

   int nRow = img.rows;
   int nCols =img.cols;

//test ok
//   imshow("test image in extractTrainingData",img);
//return;

    cocv->createMask_from_poly(img, &img_mask,  mask_pts);

    //imshow("HL: Polygon Mask Image",img_mask);

    build_trainingSet(img, img_mask, classId, selection_rect);



}

//pixel extraction algo
// 1) create a new Mat same size as image
// 2) draw filled poly on new image (with background completely black)
// 3) run through identical img to search for non black pixels (this are pixels inside selected poly

//note: mask must be of type CV_8U
void cClassifier::build_trainingSet(Mat img, Mat mask, int classId, Rect selection_rect)
{

    Mat hsv_img;
    cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);


    //Make copy all pixel values inside selected region using mask (CV_8U) image (Mat) (mask indicates which non-zero elements should be copied)
    Mat hsv_trainingData, temp_data;
    hsv_img.copyTo(temp_data, mask);  //makes copy size of orig image; all pixels not in mask are black; so copies just pixels of interest spec'd by roi (provided by mask)


    Mat timg;
    img.copyTo(timg,mask);

    //select sub-region (need a rect region for this)
    //Rect rect(0,0,640,480);
    Mat timg_region = timg(selection_rect);  //note: rect will have to be same aspect ratio as 640:480 to avoid distortion...

    //resize sub-image to 640x480
    //Size size(640,480);
    //cv::resize(timg_region,timg,size);

    imshow("Selected Region",timg_region);


    //int n_pixels = cv::countNonZero(hsv_trainingData);  //don't do this on color image

   // imshow("hsv_trainingData", temp_data);

// PLOT HISTOGRAM OF CAPTURED REGION ONLY; This needs work, but we want to do this...
    cocv->plot_hsv_histogram( img, mask );
    //cocv->plot_hs_histogram( timg );

    //now have to extract pixels we want


    //get pixel count of our class
    int nRows, nCols;

    nRows = mask.rows;
    nCols = mask.cols;

    int n_pixels = cv::countNonZero(mask==255);
    int blk_pixels = cv::countNonZero(mask==0);

    int total_mask_pixels = nRows*nCols;

    int white_pixels = total_mask_pixels - blk_pixels;


    nRows = img.rows;
    nCols = img.cols;


    unsigned int h,s,v, b,g,r;
    unsigned int hot_pixel_count = 0;

    float sum_h, sum_s, sum_v;


    Point pt;
    Point3i pt3d, hsv_pt3d, bgr_pt3d;
    int label;



    for(int y=0;y<nRows;y++)
    {
        for(int x=0;x<nCols;x++)
        {
            unsigned int mask_colr = mask.at<unsigned char>(Point(x,y));

            if(mask_colr == 255 )
            {
                Vec3b colour = hsv_img.at<Vec3b>(Point(x,y));
                Vec3b bgr_colour = img.at<Vec3b>(Point(x,y));

                if (hot_pixel_count > n_pixels)
                {
                    //flag error
                    QMessageBox msgBox;
                    msgBox.critical(0, "Error", "Too many selected pixels inside build_trainingSet in cclassifier");
                    return;
                }

                b = bgr_colour[0];
                g = bgr_colour[1];
                r = bgr_colour[2];

                h = colour[0];
                s = colour[1];
                v = colour[2];

                sum_h += (float)h;
                sum_s += (float)s;
                sum_v += (float)v;

                if(use_three_colors)
                {
                    //pt3d.x = h;
                    //pt3d.y = s;
                    //pt3d.z = v;
                    //master_ccd.add_pixelData(pt3d,classId);

                    hsv_pt3d.x = h;
                    hsv_pt3d.y = s;
                    hsv_pt3d.z = v;
                    bgr_pt3d.x = b;
                    bgr_pt3d.y = g;
                    bgr_pt3d.z = r;
                    master_ccd.add_pixelData(bgr_pt3d,hsv_pt3d,classId);

                }
                else
                {
                    pt.x = h;
                    pt.y = s;
                    master_ccd.add_pixelData(pt,classId);
                }


                hot_pixel_count++;
            }
        }
    }


    //check
    if (hot_pixel_count != n_pixels)
    {
        //flag error
        QMessageBox msgBox;
        msgBox.critical(0, "Error", "#selected pixels don't match # in mask; inside build_trainingSet in cclassifier");
        return;
    }



    //Mat tDataMat(n_pixels, 2, CV_32FC1, tData);
    //trainingData =  tDataMat;

    float mean_h = sum_h / (float)n_pixels;
    float mean_s = sum_s / (float)n_pixels;
    float mean_v = sum_v / (float)n_pixels;


    //imshow("trainingData",trainData);  //doesn't want to do this; need another way to visulize data
    //imshow("trainingData 2",trainData);


return;



/* really old
    //remove 'v' channel from data

    vector<Mat> hsvChannels;
    vector<Mat> hsChannels;
    //Mat hsvChannels[3];
    split(hsv_trainingData, hsvChannels);

    hsChannels.push_back(hsvChannels[0]);
    hsChannels.push_back(hsvChannels[2]);

    // Merge the two channels (supposed to work on only two channels)
    merge(hsChannels, *trainingData);
*/

    //get count of # of color'd pixels [should match 'pixelCount'
    //int num_hotPixels = cv::countNonZero(mask);


    //search through img to find non-black pixels; add them to list
    //cv::Mat locations;   // output, locations of non-zero pixels
    //cv::findNonZero(img, locations);  // ************* this returns the pixel values from non-zero pixels

    // access pixel coordinates
/*
    int nPixels = locations.rows;  //this should match num_hotPixels

    if(nPixels == num_hotPixels)
    {
        Point img_pnt, list_pnt; // = locations.at<Point>(i);
        int x; // = pnt.x;
        int y; // = pnt.y;

        unsigned int h,s,v;
        float H,S,V;

        list_pnt.x = 1;

        for(int i=0;i<num_hotPixels;i++)
        {
            img_pnt = locations.at<Point>(i);
            list_pnt.y = i;


            //get HSV pixel value
            Vec3b colour = img.at<Vec3b>(img_pnt);
            //Vec3b colour = img.at<Vec3b>(Point(x, y));
            //Vec3b bgrPixel = img.at<Vec3b>(i, j);

            h =  colour.val[0];
            s =  colour.val[1];
            //v =  colour.val[2];


            //img.at<cv::Vec3b>(y,x)[0];  //to access blue at y,x
            //pixel_list.at<CV_8UC3>(pnt) = colour;

            //cleanest way for direct pixel assign; however we're not doing this; so just ref code for later
            //trainingData.at<CV_8UC3>(list_pnt) = img.at<Vec3b>(img_pnt);


            trainingData.at<Vec2b>(list_pnt)[0] = h;
            trainingData.at<Vec2b>(list_pnt)[1] = s;

            //trainingLabels.at<cv::Vec>(list_pnt)[0] = classId;


        }
    }
*/

}

//save Mat as XML file
//send a file path + name (no extension); this will add extension and tack on Data.xml and Labels.xml
void cClassifier::save_TrainingSet(QString fileName, Mat trainingData, Mat trainingLabels)
{
    QString qFname1 = fileName + "Data.xml";
    QString qFname2 = fileName + "Labels.xml";

    // convert QString to std::string
    std::string fname1 = qFname1.toUtf8().constData();
    std::string fname2 = qFname2.toUtf8().constData();


    // write Mat to file
    cv::FileStorage fs1(fname1, cv::FileStorage::WRITE);
    fs1 << "TrainingData" << trainingData;

    cv::FileStorage fs2(fname2, cv::FileStorage::WRITE);
    fs2 << "TrainingLabels" << trainingLabels;

    //alternative is to simply using
    //SAVE image
    //imwrite("result.jpg",image);// it will store the image in name "result.jpg"

}

//Load Mat XML file
//send a file path + name (no extension); this will add extension and tack on Data.xml and Labels.xml
void cClassifier::load_TrainingSet(QString fileName, Mat &trainingData, Mat &trainingLabels)
{
    Mat tData, tLabels;

    QString qFname1 = fileName + "Data.xml";
    QString qFname2 = fileName + "Labels.xml";

    // convert QString to std::string
    std::string fname1 = qFname1.toUtf8().constData();
    std::string fname2 = qFname2.toUtf8().constData();


    // read Mat from file
    cv::FileStorage fs1(fname1, FileStorage::READ);
    fs1["TrainingData"] >> tData;

    cv::FileStorage fs2(fname2, FileStorage::READ);
    fs2["TrainingLabels"] >> tLabels;

    trainingData = tData;
    trainingLabels = tLabels;
}

void cClassifier::save_Bayes_Classifier(QString fileName, Mat trainingData, Mat trainingLabels)
{
    // Set up Bayes's parameters
    //works??
    Ptr<ml::NormalBayesClassifier> bayesCl(ml::NormalBayesClassifier::create());
    //Ptr<cv::ml::TrainData> trData = cv::ml::TrainData::create(trainingData,ml::SampleTypes::ROW_SAMPLE,trainingLabels);
    //bayes->train(trData);
    bayesCl->train(trainingData,ml::SampleTypes::ROW_SAMPLE,trainingLabels);

    //save model for later

    // convert QString to std::string
    std::string fname = fileName.toUtf8().constData();

    bayesCl->save(fname);

}


void cClassifier::load_Bayes_Classifier(QString fileName)
{

    //now load it from file
    //Ptr<ml::NormalBayesClassifier> bayesCl_new(ml::NormalBayesClassifier::create());
    Ptr<ml::NormalBayesClassifier> bayesCl_new;
    bayesCl_new = NormalBayesClassifier::create();

    // convert QString to std::string
    const std::string fname = fileName.toUtf8().constData();
    const std::string snull = "";
    const cv::String cvStr[] = {"bayesClassifier.xml"};

//    bayesCl_new->load(fname);
//    bayesCl_new = NormalBayesClassifier::load(fname);
//    bayesCl_new = NormalBayesClassifier::load("bayesClassifier.xml");
//    bayesCl_new = NormalBayesClassifier::load(cvStr);

    bayesCl_new = NormalBayesClassifier::load<NormalBayesClassifier>(fname);

}


// accuracy
float cClassifier::evaluate(cv::Mat& predicted, cv::Mat& actual) {
    assert(predicted.rows == actual.rows);
    int t = 0;
    int f = 0;
    for(int i = 0; i < actual.rows; i++) {
        float p = predicted.at<float>(i,0);
        float a = actual.at<float>(i,0);
        if((p >= 0.0 && a >= 0.0) || (p <= 0.0 &&  a <= 0.0)) {
            t++;
        } else {
            f++;
        }
    }
    return (t * 1.0) / (t + f);
}

// plot data and class
void cClassifier::plot_binary(cv::Mat& data, cv::Mat& classes, string name) {
    cv::Mat plot(size, size, CV_8UC3);
    plot.setTo(cv::Scalar(255.0,255.0,255.0));
    for(int i = 0; i < data.rows; i++) {

        float x = data.at<float>(i,0) * size;
        float y = data.at<float>(i,1) * size;

        if(classes.at<float>(i, 0) > 0) {
            cv::circle(plot, Point(x,y), 2, CV_RGB(255,0,0),1);
        } else {
            cv::circle(plot, Point(x,y), 2, CV_RGB(0,255,0),1);
        }
    }
    cv::imshow(name, plot);
}




void cClassifier::bayes(cv::Mat& training_Data, cv::Mat& training_Classes, cv::Mat& test_Data, cv::Mat& test_Classes)
{
    //Ptr<cv::ml::NormalBayesClassifier> bayesClassifier(trainingData, trainingClasses);

    Ptr<cv::ml::NormalBayesClassifier> bayesClassifier = cv::ml::NormalBayesClassifier::create();
    bayesClassifier->train(training_Data,cv::ml::ROW_SAMPLE,training_Classes);


    //cv::Mat predicted(testClasses.rows, 1, CV_32F);
    //cv::Mat predicted(1, 1, CV_32F);
    cv::Mat predicted(test_Data.rows, 1, CV_32S);
    cv::Mat pred(1, 1, CV_32S);

    for (int i = 0; i < test_Data.rows; i++)
    {
        const cv::Mat sample = test_Data.row(i);
        //predicted.at<float> (i, 0) = bayes.predict(sample);
        bayesClassifier->predict(sample,pred );

        unsigned int pred_label = predicted.at<unsigned int>(0,0);

        unsigned int v = pred_label;

        predicted.push_back(pred);

    }

    cout << "Accuracy_{BAYES} = " << evaluate(predicted, test_Classes) << endl;
    plot_binary(test_Data, predicted, "Predictions Bayes");
}

//note: to concat two Mat's
/*
 * Mat A,B;  //data matrices
 * Mat H,V;  //destination matrices
 *
 * hconcat(A,B,H);  //horizontal concat
 * vconcat(A,B,V);  //vertical concat
 *
 */




//note: this example is for loading at greyscale image; not a multi-channel...
cv::Mat cClassifier::load_data_from_image(QString filePathName, int class_id)
{
    cv::Mat img;

    //1) load image from file
    //read_cvImage(&img, filePathName);

    //2) convert image to HSV
    Mat cImg = img.clone();
    //Mat cImg;

    // Convert BGR to HSV
    cvtColor(img, cImg, cv::COLOR_BGR2HSV);


    //3) create correct size Mat for classification of image data
    //3b) create correct size Mat for class Id data

    //4)transfer HS values from image to target Mat for classification




    //Mat img_mat = imread("/home/buddhika/workspace/project/images/t.jpg");
    Mat img_mat = imread("/home/buddhika/workspace/project/images/t.jpg");

    // Load images in the C++ format
    Size size(64,124);//the image size,e.g.64x124
    resize(img_mat ,img_mat ,size);//resize image

    int num_files = 1;//number of images
    int img_area = 64*124;//imag size
    //initialize the training matrix
    //The number of rows in the matrix would be 5, and the number of   columns would be the area of the image, 64*124 = 12


    Mat training_mat(img_area,num_files,CV_32FC1);
    // "fill in" the rows of training_mat with the data from each image.
    cvtColor(img_mat,img_mat, CV_RGB2GRAY);
    imshow("",img_mat);

    int ii = 0; // Current column in training_mat
    //Do this for every training image
    int file_num=0;
    for (int i = 0; i<img_mat.rows; i++) {
        for (int j = 0; j < img_mat.cols; j++) {
            training_mat.at<float>(ii++,file_num) = img_mat.at<uchar>(i,j);
        }
    }
}


bool cClassifier::save_current_trainingSet(QString filePathName, QString headerLine, bool append)
{
    bool ok;

    if(ok)
        master_ccd.save_current_trainingData_to_csv(filePathName, headerLine, append);
    else
    {
        //send error box message

    }
}

bool cClassifier::save_current_3i_trainingSet(QString filePathName, QString headerLine, bool append)
{
    bool ok=true;

    ok = master_ccd.save_current_3i_trainingData_to_csv(filePathName, headerLine, append);

    return ok;
}


bool cClassifier::load_toCurrent_trainingSet(QString filePathName)
{
    vector<int> trainLabels;
    vector<Point> trainData;

    bool ok = master_ccd.read_csv_trainingData(filePathName,trainData, trainLabels);

    if(ok)
    {
        master_ccd.load_trainingData(trainData, trainLabels);
    }
    else
    {
        //send error box message

    }

    return ok;
}

bool cClassifier::load_toCurrent_3i_trainingSet(QString filePathName)
{
    vector<int> trainLabels;
    vector<Point3i> bgr_trainData;
    vector<Point3i> hsv_trainData;

    bool ok = master_ccd.read_3i_csv_trainingData(filePathName, bgr_trainData, hsv_trainData, trainLabels);

    if(ok)
    {
        master_ccd.load_trainingData(bgr_trainData, hsv_trainData, trainLabels);
    }
    else
    {
        //send error box message

    }

    return ok;
}



//have to return two Mat's, need to use Ptr's here
cv::Mat cClassifier::load_data_from_csv(QString filePathName, int headerLineCount, int class_id)  //headerLinecount is # of lines to skip at top of file
{
    QByteArray ba = filePathName.toLatin1();

    char* filename = ba.data();


    cv::Ptr<cv::ml::TrainData> raw_data = cv::ml::TrainData::loadFromCSV(filename, headerLineCount);
    cv::Mat data = raw_data->getSamples();

    raw_data->getResponses();

    return data;

}


void cClassifier::read_cvImage(cv::Mat &img, QString filePathName)
{
    QByteArray ba = filePathName.toLatin1();

    char* imageName = ba.data();

    //Mat img;
    img = imread( imageName, 1 );

}

/*
vector<Point3f> cClassifier::convert_mat_to_point_array(Mat a)
{
    //Mat a = Mat::ones(10,1,CV_32FC3);
    vector<Point3f> b;
    a.copyTo(b);

    return b;
}

vector<Point2f> cClassifier::convert_mat_to_point_array(Mat a)
{
    //Mat a = Mat::ones(10,1,CV_32FC3);
    vector<Point2f> b;
    a.copyTo(b);

    return b;
}
*/

QString cClassifier::get_openCv_type(int type)
{
    //int type = output.type();  //usage; Mat output

    //cv::Size s = output.size();
    //int rows = s.height;
    //int cols = s.width;


    uchar depth = type & CV_MAT_DEPTH_MASK;

    QString r;

    switch(depth)
    {
        case CV_8U: r = "8U"; break;
        case CV_8S: r = "8S"; break;
        case CV_16U: r = "16U"; break;
        case CV_16S: r = "16S"; break;
        case CV_32S: r = "32S"; break;
        case CV_32F: r = "32F"; break;
        case CV_64F: r = "64F"; break;
        default:     r = "User"; break;

    }

    return r;
}


//test simple NNet (2 hidden layers of 60 neurons each)
void cClassifier::test7()
{
    const int hiddenLayerSize = 60;
     float inputTrainingDataArray[4][2] = {
         { 0.0, 0.0 },
         { 0.0, 1.0 },
         { 1.0, 0.0 },
         { 1.0, 1.0 }
     };
     Mat inputTrainingData = Mat(4, 2, CV_32F, inputTrainingDataArray);

     float outputTrainingDataArray[4][1] = {
         { 0.0 },
         { 1.0 },
         { 1.0 },
         { 0.0 }
     };
     Mat outputTrainingData = Mat(4, 1, CV_32F, outputTrainingDataArray);

     Ptr<ANN_MLP> mlp = ANN_MLP::create();

     Mat layersSize = Mat(4, 1, CV_16U);
     layersSize.row(0) = Scalar(inputTrainingData.cols);
     layersSize.row(1) = Scalar(hiddenLayerSize);
     layersSize.row(2) = Scalar(hiddenLayerSize);
     layersSize.row(3) = Scalar(outputTrainingData.cols);
     mlp->setLayerSizes(layersSize);

     mlp->setActivationFunction(ANN_MLP::ActivationFunctions::SIGMOID_SYM);

     TermCriteria termCrit = TermCriteria(
         TermCriteria::Type::COUNT + TermCriteria::Type::EPS,
         900,
         0.0001
     );
     mlp->setTermCriteria(termCrit);

     mlp->setTrainMethod(ANN_MLP::TrainingMethods::BACKPROP, 0.0001);  //adjust 2nd term smaller if not learning

     Ptr<TrainData> trainingData = TrainData::create(
         inputTrainingData,
         SampleTypes::ROW_SAMPLE,
         outputTrainingData
     );

     mlp->train(trainingData
        /* , ANN_MLP::TrainFlags::UPDATE_WEIGHTS
         + ANN_MLP::TrainFlags::NO_INPUT_SCALE
         + ANN_MLP::TrainFlags::NO_OUTPUT_SCALE */
     );

     for (int i = 0; i < inputTrainingData.rows; i++)
     {
         Mat sample = Mat(1, inputTrainingData.cols, CV_32F, inputTrainingDataArray[i]);
         Mat result;
         mlp->predict(sample, result);
         //cout << sample << " -> ";// << result << endl;
         //print(result, 0);
         //cout << endl;
     }
}


//Create a Quad-Color Test Image and save to file
void cClassifier::test6()
{
    int width=160, height=240;

    uchar redVal=255, greenVal=255,blueVal=255;
    Mat imgWhite = cocv->create_color_mat(width, height, blueVal, greenVal, redVal);

    redVal=0, greenVal=0,blueVal=255;
    Mat imgBlue = cocv->create_color_mat(width, height, blueVal, greenVal, redVal);

    redVal=255, greenVal=0,blueVal=0;
    Mat imgRed = cocv->create_color_mat(width, height, blueVal, greenVal, redVal);

    redVal=0, greenVal=255,blueVal=0;
    Mat imgGreen = cocv->create_color_mat(width, height, blueVal, greenVal, redVal);

    redVal=255, greenVal=255,blueVal=0;
    Mat imgYellow = cocv->create_color_mat(width, height, blueVal, greenVal, redVal);

    redVal=131, greenVal=94,blueVal=80;
    Mat imgBrown = cocv->create_color_mat(width, height, blueVal, greenVal, redVal);

    redVal=0, greenVal=0,blueVal=0;
    Rect dstRect(0,0,640,480);
    Mat imgBlack = cocv->create_color_mat(dstRect, blueVal, greenVal, redVal);

    Rect dstRect1(0,0,width,height);
    Rect dstRect2(0,height,width,height);
    Rect dstRect3(width,0,width,height);
    Rect dstRect4(width,height,width,height);
    Rect dstRect5(2*width,0,width,height);
    Rect dstRect6(2*width,height,width,height);
    Rect srcRect(0,0,width,height);

    cocv->copy_srcMatRect_to_dstMatRect(imgBlue, imgBlack, srcRect, dstRect1);
    cocv->copy_srcMatRect_to_dstMatRect(imgRed, imgBlack, srcRect, dstRect2);
    cocv->copy_srcMatRect_to_dstMatRect(imgGreen, imgBlack, srcRect, dstRect3);
    cocv->copy_srcMatRect_to_dstMatRect(imgYellow, imgBlack, srcRect, dstRect4);
    cocv->copy_srcMatRect_to_dstMatRect(imgWhite, imgBlack, srcRect, dstRect5);
    cocv->copy_srcMatRect_to_dstMatRect(imgBrown, imgBlack, srcRect, dstRect6);

    imshow("create color image", imgBlack);

    cocv->saveImage(imgBlack);


}




//example on how to create image mask [also tests paint_poly_to_image (color)]
void cClassifier::test5()
{


    // Data for visual representation
    int width = 512, height = 512;
    Mat src_image = Mat::ones(height, width, CV_8UC3);  //create 3 channel image

    cv::Scalar back_clr(222,254,50);

    //add some colr
    src_image.setTo(back_clr);


    imshow("source iamage", src_image);

    Mat img_mask, dest_image;

    vector<Point> mask_pts;
    mask_pts.push_back(Point(100,100));
    mask_pts.push_back(Point(100,200));
    mask_pts.push_back(Point(200,200));
    mask_pts.push_back(Point(200,100));

    cv::Scalar clr(0,255,0);

    cocv->paint_poly_to_image(src_image, &dest_image, mask_pts, clr);

    //fill poly into image
    cocv->createMask_from_poly(src_image, &img_mask, mask_pts);


}

//3D color test (tested works)
void cClassifier::test4()
{
    // Data for visual representation
    int width = 512, height = 512;
    Mat image = Mat::zeros(height, width, CV_8UC3);

    // Set up training data
    int nsamples =10;
    int nclr_chans = 3;

    //int training_Labels[nrows] = {1, -1, -1, -1, -1, 1, -1, 1, 1, 1};
    int training_Labels[nsamples] = {1, -1, -1, -1, -1, 1, 1, -1, -1, 1};
    Mat trainingLabelsMat(1,nsamples, CV_32SC1, training_Labels);

    //                                            1-white         2-yellow         3-blue      4-green       5-low_red      6-brown      7-shad.cot    8-high-red     9-dark-blue   10-light-bur
    //          (OCV HSV; so Hue  0-180) HSV = { { 81,  9,100}, { 33, 33, 80}, {112, 60, 99}, { 60,100,100}, { 1,60,23}, {  0, 33, 71}, {101, 11, 80}, {178, 49, 81}, {115,52,55} , { 53, 10,100}};
    //          (Gimp HSV; so Hue 0-359) HSV = { {162,  9,100}, { 67, 33, 80}, {225, 60, 99}, {120,100,100}, { 2,60,23}, {  0, 33, 71}, {201, 11, 80}, {358, 49, 81}, {231,52,55} , {106, 10,100}};
    float trainingData[nsamples][nclr_chans] = { {248,255,231}, {137,203,195}, {252,138,100}, {  0,255,0  }, {23,24,58}, {120,120,180}, {203,195,180}, {108,105,206}, {140,78,67} , {230,255,236}};
    Mat trainingDataMat(nsamples, nclr_chans, CV_32FC1, trainingData);


    for(int i=0;i<nsamples;i++)
    {
        Point3i pt;
        int label;

        label = training_Labels[i];
        pt.x = trainingData[i][0];
        pt.y = trainingData[i][1];
        pt.z = trainingData[i][2];
        //master_ccd.add_pixelData(pt,label);
        master_ccd.add_pixelData(pt,pt,label);  //changed it to accept hsv and bgr data
    }

    Mat ccd_dataMat, ccd_labelsMat;
    master_ccd.get_mat_from_list(ccd_dataMat, ccd_labelsMat);


    // Set up Bayes's parameters
    //works??
    Ptr<ml::NormalBayesClassifier> bayesCl(ml::NormalBayesClassifier::create());
    //bayesCl->train(trainingDataMat,ml::SampleTypes::ROW_SAMPLE,trainingLabelsMat);

    bayesCl->train(ccd_dataMat,ml::SampleTypes::ROW_SAMPLE,ccd_labelsMat);


    //back predict
    for(int i=0;i<nsamples;i++)
    {
        Point3i pt;
        int label;
        float b,g,r;

        b = trainingData[i][0];
        g = trainingData[i][1];
        r = trainingData[i][2];

        //ck read out from Mat
        b = ccd_dataMat.at<float>(i,0);
        g = ccd_dataMat.at<float>(i,1);
        r = ccd_dataMat.at<float>(i,2);

        Mat sampleMat = (Mat_<float>(1,3) << b,g,r);

        //test output order is as expected
        b = sampleMat.at<float>(i,0);
        g = sampleMat.at<float>(i,1);
        r = sampleMat.at<float>(i,2);


        Mat output, outputProb;
        bayesCl->predictProb(sampleMat,output, outputProb);
        //float response = output.at<float>(0,0);
        int response = output.at<int>(0,0);

        //                            i = {0,  1,  2,  3,  4, 5, 6,  7,  8, 9}
        //int training_Labels[nsamples] = {1, -1, -1, -1, -1, 1, 1, -1, -1, 1};
        int x = response;
    }

}


//good example for creating and using Bayes Classifier
//may need sufficient data for Bayes Classifier (hence really good to plot the space out; we barely have enough in this example
void cClassifier::test3()
{
    // Data for visual representation
    int width = 512, height = 512;
    Mat image = Mat::zeros(height, width, CV_8UC3);

    // Set up training data
    int nrows =10;
    int ncols = 2;

    //int training_Labels[nrows] = {1, -1, -1, -1, -1, 1, -1, 1, 1, 1};
    int training_Labels[nrows] = {1, 0, 0, 0, 0, 1, 0, 1, 1, 1};
    Mat trainingLabels(nrows, 1, CV_32SC1, training_Labels);
    Mat t;
    cv::transpose(trainingLabels, t);

    Mat trainingLabelsMat;
    cv::transpose(t,trainingLabelsMat);


    float trainingData[nrows][ncols] = { {501, 10}, {255, 10}, {501, 255}, {10, 501}, {250, 240}, {370, 120}, {400, 250}, {400, 25}, {450, 5} , {480, 15}};
    Mat trainingDataMat(nrows, ncols, CV_32FC1, trainingData);


    for(int i=0;i<nrows;i++)
    {
        Point pt;
        int label;

        label = training_Labels[i];
        pt.x = trainingData[i][0];
        pt.y = trainingData[i][1];
        master_ccd.add_pixelData(pt,label);
    }

    Mat ccd_dataMat, ccd_labelsMat;
    master_ccd.get_mat_from_list(ccd_dataMat, ccd_labelsMat);


    // Set up Bayes's parameters
    //works??
    Ptr<ml::NormalBayesClassifier> bayesCl(ml::NormalBayesClassifier::create());
    //Ptr<cv::ml::TrainData> trData = cv::ml::TrainData::create(trainingData,ml::SampleTypes::ROW_SAMPLE,trainingLabels);
    //bayes->train(trData);
    bayesCl->train(trainingDataMat,ml::SampleTypes::ROW_SAMPLE,trainingLabelsMat);

    bayesCl->train(ccd_dataMat,ml::SampleTypes::ROW_SAMPLE,ccd_labelsMat);

    //save model for later
    bayesCl->save("trainedBayes.xml");

    //now load it from file
    Ptr<ml::NormalBayesClassifier> bayesCl_new(ml::NormalBayesClassifier::create());
    //bayesCl_new = NormalBayesClassifier::load("trainedBayes.xml");
    bayesCl_new = NormalBayesClassifier::load<NormalBayesClassifier>("trainedBayes.xml");



    //Paint out to image regions as classed color for location (training data is 2d vector from 0:512)

    Vec3b green(0,255,0), blue (255,0,0);
    // Show the decision regions given by the SVM
    for (int i = 0; i < image.rows; ++i)
    {
        for (int j = 0; j < image.cols; ++j)
        {
            Mat sampleMat = (Mat_<float>(1,2) << j,i);
            //float response = svm->predict(sampleMat);
            //Mat sampleMat = (Mat_<float>(1,2) << trainingData[i][0],trainingData[i][1]);



            Mat output, outputProb;
            bayesCl->predictProb(sampleMat,output, outputProb);
            //float response = output.at<float>(0,0);
            int response = output.at<int>(0,0);

            int x = trainingData[0][0];
            int y = trainingData[0][1];

            if((x==j)&&(y==i))
            {
                int z = x;
            }

            if (response == 1)
                image.at<Vec3b>(i,j)  = green;
            else
                image.at<Vec3b>(i,j)  = blue;
        }
    }


    // Show the training data
    int thickness = 1;
    int lineType = 8;
    cv::Scalar red(0,0,255);
    cv::Scalar yellow(0,255,255);
    cv::Scalar clr;

    for (int i = 0; i < nrows; ++i)
    {
        Point center;
        center.x = (int)trainingData[i][0];
        center.y = (int)trainingData[i][1];

        //get prediction for data
        Mat sampleMat = (Mat_<float>(1,2) << trainingData[i][0],trainingData[i][1]);
        //float response = svm->predict(sampleMat);

        Mat output, outputProb;
        bayesCl->predictProb(sampleMat,output, outputProb);
        int response = output.at<int>(0,0);

        if( response == training_Labels[i] )
        {
            //correct response; paint it yellow
            clr = yellow;
        }
        else
        {
            //incorrect; flag it red
            clr = red;
        }

        circle( image,  center,   3,  clr, thickness, lineType);
    }


/*
    // Show support vectors
    thickness = 2;
    lineType  = 8;
    Mat sv = svm->getSupportVectors();

    for (int i = 0; i < sv.rows; ++i)
    {
        const float* v = sv.ptr<float>(i);
        circle( image,  Point( (int) v[0], (int) v[1]),   6,  Scalar(128, 128, 128), thickness, lineType);
    }
*/

    // save the image
    imwrite("result.png", image);

    // show it to the user
    imshow("SVM Simple Example", image);

    //waitKey(0);
}

void cClassifier::test2()
{
    // Data for visual representation
    int width = 512, height = 512;
    Mat image = Mat::zeros(height, width, CV_8UC3);

    // Set up training data
    int nrows =7;
    int ncols = 2;

    int trainingLabels[nrows] = {1, -1, -1, -1, -1, 1, -1};
    Mat trainingLabelsMat(nrows, 1, CV_32SC1, trainingLabels);

    float trainingData[nrows][ncols] = { {501, 10}, {255, 10}, {501, 255}, {10, 501}, {250, 240}, {370, 120}, {400, 250} };
    Mat trainingDataMat(nrows, ncols, CV_32FC1, trainingData);


    // Set up SVM's parameters
    //works
    Ptr<ml::SVM> svm(ml::SVM::create());
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER,100,1e-6));

    svm->train(trainingDataMat,ml::SampleTypes::ROW_SAMPLE,trainingLabelsMat);



    //Paint out to image regions as classed color for location (training data is 2d vector from 0:512)

    Vec3b green(0,255,0), blue (255,0,0);
    // Show the decision regions given by the SVM
    for (int i = 0; i < image.rows; ++i)
    {
        for (int j = 0; j < image.cols; ++j)
        {
            Mat sampleMat = (Mat_<float>(1,2) << j,i);
            float response = svm->predict(sampleMat);

            if (response == 1)
                image.at<Vec3b>(i,j)  = green;
            else if (response == -1)
                image.at<Vec3b>(i,j)  = blue;
        }
    }


    // Show the training data
    int thickness = 2;
    int lineType = 8;
    cv::Scalar red(0,0,255);
    cv::Scalar yellow(0,255,255);
    cv::Scalar clr;

    for (int i = 0; i < nrows; ++i)
    {
        Point center;
        center.x = (int)trainingData[i][0];
        center.y = (int)trainingData[i][1];

        //get prediction for data
        Mat sampleMat = (Mat_<float>(1,2) << trainingData[i][0],trainingData[i][1]);
        float response = svm->predict(sampleMat);

        if( ((int)response > 0)&&(trainingLabels[i] > 0) )
        {
            //correct response; paint it yellow
            clr = yellow;
        }
        else if( ((int)response <= 0)&&(trainingLabels[i] <= 0) )
        {
            //correct response; paint it yellow
            clr = yellow;
        }
        else
        {
            //incorrect; flag it red
            clr = red;
        }

        circle( image,  center,   3,  clr, thickness, lineType);
    }


/*
    // Show support vectors
    thickness = 2;
    lineType  = 8;
    Mat sv = svm->getSupportVectors();

    for (int i = 0; i < sv.rows; ++i)
    {
        const float* v = sv.ptr<float>(i);
        circle( image,  Point( (int) v[0], (int) v[1]),   6,  Scalar(128, 128, 128), thickness, lineType);
    }
*/

    // save the image
    imwrite("result.png", image);

    // show it to the user
    imshow("SVM Simple Example", image);

    //waitKey(0);
}


int cClassifier::test()
{

    Mat_<float> trainingData(10,2);
    trainingData << 0,0,
                    0.5,0,
                    0.5,0.25,
                    1,1.25,
                    1,1.5,
                    1,1,
                    0.5,1.5,
                    0.25,1,
                    2,1.5,
                    2,2.5;
    Mat_<int> trainingLabels(1,10);
    trainingLabels << 0,0,0,1,1,1,0,1,1,1;

    float t_max = 2.5;
    trainingData = trainingData/t_max;  //normalize data 0:1 (have to for floats for plotting)


    plot_binary(trainingData, trainingLabels, "Training Data");
    //plot_binary(testData, testClasses, "Test Data");



    Mat_<float> testData(1,2);
    testData << 0.1,0.2;

    Mat_<int> testLabels(1,1);
    testLabels << 1;

    testData = testData/t_max;  //normalize data 0:1 (have to for floats for plotting)


    //plot_binary(testData, testLabels, "Test Data");


// Knn

    Ptr<ml::KNearest> knn(ml::KNearest::create());
    knn->train(trainingData,ml::SampleTypes::ROW_SAMPLE,trainingLabels);


    int K=8;
    Mat nearestNeighbors, distances;
    knn->findNearest(testData,K,noArray(),nearestNeighbors,distances);

    for(int i=0;i<nearestNeighbors.rows;i++)
    {
        const cv::Mat sample = nearestNeighbors.row(i);
        //float x = sample.col(0);
        //float y = sample.col(1);
        float x = sample.at<cv::Vec2f>(i)[0];
        float y = sample.at<cv::Vec2f>(i)[1];

        //img.at<cv::Vec3b>(y,x)[2] = r;

    }



// Svm
    Ptr<ml::SVM> svm(ml::SVM::create());
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER,100,1e-6));

    svm->train(trainingData,ml::SampleTypes::ROW_SAMPLE,trainingLabels);

    Mat res;
    svm->predict(testData, res);

    int closest_class = res.at<float>(0);



//Bayes
    Ptr<ml::NormalBayesClassifier> bayesCl(ml::NormalBayesClassifier::create());
    //Ptr<cv::ml::TrainData> trData = cv::ml::TrainData::create(trainingData,ml::SampleTypes::ROW_SAMPLE,trainingLabels);
    //bayes->train(trData);
    bayesCl->train(trainingData,ml::SampleTypes::ROW_SAMPLE,trainingLabels);

    Mat output, outputProb;
    bayesCl->predictProb(testData,output, outputProb);

    unsigned int pred_label = output.at<unsigned int>(0,0);



    bayes(trainingData, trainingLabels, testData, testLabels);


    //cv::waitKey();

    return 0;
}
