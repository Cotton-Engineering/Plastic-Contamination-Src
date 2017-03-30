#include "cclassifierdata.h"

#include "cutils.h"

#include <algorithm>  //for C++98 onwards


cClassifierData::cClassifierData(QObject *parent) : QObject(parent)
{

}

//chks if bi-modal; if so; adds 2 pixels to list
void cClassifierData::add_pixelData(Point pt, int classId)
{
    master_data_list.push_back(pt);
    master_label_list.push_back(classId);

    length = static_cast<long unsigned int>(master_data_list.size());

    int x=0;
    use_three_colors = false;
}
/*
void cClassifierData::add_pixelData(Point3i pt, int classId )
{
    master_data3i_list.push_back(pt);
    master_label3i_list.push_back(classId);

    length = static_cast<long unsigned int>(master_data3i_list.size());

    int x=0;
    use_three_colors = true;
}
*/
void cClassifierData::add_pixelData(Point3i bgr_pt, Point3i hsv_pt, int classId )
{
    master_bgr_data3i_list.push_back(bgr_pt);
    master_data3i_list.push_back(hsv_pt);
    master_label3i_list.push_back(classId);

    length = static_cast<long unsigned int>(master_data3i_list.size());

    int x=0;
    use_three_colors = true;
}

bool cClassifierData::get_mat_from_list(vector<Point3i> &trainData3i, vector<int> &trainLabels3i, Mat &tDataMat, Mat &tLabelsMat)
{
    Mat pt_mat_list;
    Mat labelMat_list;

    pt_mat_list = cv::Mat(trainData3i);
    labelMat_list = cv::Mat(trainLabels3i).reshape(1);

    int nchans = pt_mat_list.channels();

    Mat flat_pt_mat_list  = pt_mat_list.reshape(1);  //reshape for only 1 channel

    nchans = flat_pt_mat_list.channels();

    flat_pt_mat_list.convertTo(tDataMat, CV_32FC1);


    //Mat labelMat_list = cv::Mat(master_label_list).reshape(1);
    nchans = labelMat_list.channels();

    labelMat_list.convertTo(tLabelsMat, CV_32SC1);

    int x = 0;
}


//returns data in format ready for submission to classifiers
bool cClassifierData::get_mat_from_list(Mat &tDataMat, Mat &tLabelsMat)
{

    //to convert from Vector<Point> to Mat; use constructor; however it'll create 2 channels, so also need to call reshape
    //vector<Point> master_data_list;
    //vector<int> master_label_list;

    //int trainingLabels[nrows] = {1, -1, -1, -1, -1, 1, -1};
    //Mat trainingLabelsMat(nrows, 1, CV_32SC1, trainingLabels);

    //float trainingData[nrows][ncols] = { {501, 10}, {255, 10}, {501, 255}, {10, 501}, {250, 240}, {370, 120}, {400, 250} };
    //Mat trainingDataMat(nrows, ncols, CV_32FC1, trainingData);

    Mat pt_mat_list;
    Mat labelMat_list;

    if(use_three_colors)
    {
        pt_mat_list = cv::Mat(master_data3i_list);
        labelMat_list = cv::Mat(master_label3i_list).reshape(1);
    }
    else
    {
        pt_mat_list = cv::Mat(master_data_list);
        labelMat_list = cv::Mat(master_label_list).reshape(1);
    }

    int nchans = pt_mat_list.channels();

    Mat flat_pt_mat_list  = pt_mat_list.reshape(1);  //reshape for only 1 channel

    nchans = flat_pt_mat_list.channels();

    flat_pt_mat_list.convertTo(tDataMat, CV_32FC1);


    //Mat labelMat_list = cv::Mat(master_label_list).reshape(1);
    nchans = labelMat_list.channels();

    labelMat_list.convertTo(tLabelsMat, CV_32SC1);

    int x = 0;
}

void cClassifierData::clear_all()
{

    master_data_list.clear();
    master_label_list.clear();

    master_bgr_data3i_list.clear();
    master_data3i_list.clear();
    master_label3i_list.clear();

}

/*
bool classSortComparisionOp(int indx_ptA, int indx_ptB)
{
  bool isGreaterThan = true;



  return isGreaterThan;
}

bool sort_list_by_class(int classId)
{
    sort(master_data_list.begin, master_data_list.end, classSortComparisonOp);
}
*/


void cClassifierData::sort_class_lists()
{
    //find start and end classIds
    int start_indx = *std::min_element(master_label_list.begin(), master_label_list.end());
    int stop_indx = *std::max_element(master_label_list.begin(), master_label_list.end());

    vector<int> t_label_list;
    vector<Point> t_data_list;


    int N = static_cast<int>(master_label_list.size());

    //iterate through list from lowest to highest classId
    for(int i=start_indx;i<stop_indx;i++)
    {
        int label = i;
        for(int j=0;j<N;j++)
        {
            int tLabel = master_label_list[j];
            if(label==tLabel)
            {
                Point pt = master_data_list[j];
                t_data_list.push_back(pt);
                t_label_list.push_back(label);
            }
        }

    }

    //list should now be sorted by classId

    master_label_list.clear();
    master_data_list.clear();

    master_label_list = t_label_list;  //make deep copy (no ptrs involved so it is a deep copy)
    master_data_list = t_data_list;

}


void cClassifierData::sort_class_3iLists()
{
    //find start and end classIds
    int start_indx = *min_element(master_label3i_list.begin(), master_label3i_list.end());
    int stop_indx = *max_element(master_label3i_list.begin(), master_label3i_list.end());

    vector<int> t_label_list;
    vector<Point3i> t_bgr_data_list, t_hsv_data_list;


    int N = static_cast<int>(master_label3i_list.size());

    //iterate through list from lowest to highest classId
    for(int i=start_indx;i<=stop_indx;i++)
    {
        int label = i;
        for(int j=0;j<N;j++)
        {
            int tLabel = master_label3i_list[j];
            if(label==tLabel)
            {
                Point3i hsv_pt = master_data3i_list[j];
                Point3i bgr_pt = master_bgr_data3i_list[j];
                t_hsv_data_list.push_back(hsv_pt);
                t_bgr_data_list.push_back(bgr_pt);
                t_label_list.push_back(label);
            }
        }

    }

    //list should now be sorted by classId

    master_label3i_list.clear();
    master_data3i_list.clear();
    master_bgr_data3i_list.clear();

    master_label3i_list = t_label_list;  //make deep copy (no ptrs involved so it is a deep copy)
    master_data3i_list = t_hsv_data_list;
    master_bgr_data3i_list = t_bgr_data_list;

}


bool cClassifierData::find_first_last_indx_classId(int classId, int &start_indx, int &stop_indx)
{
    bool ok = false;

    //first make sure lists are sorted
    sort_class_lists();

    int N = static_cast<int>(master_label_list.size());

    bool first_occurance_found = false;

    for(int j=0;j<N;j++)
    {
        int tLabel = master_label_list[j];
        if(classId==tLabel)
        {
            ok = true;

            if(first_occurance_found==false)
            {
                start_indx=j;
            }
            stop_indx = j;
        }
    }

    return ok;
}

bool cClassifierData::find_first_last_3i_indx_classId(int classId, int &start_indx, int &stop_indx)
{
    bool ok = false;

    //first make sure lists are sorted
    sort_class_3iLists();

    int N = static_cast<int>(master_label3i_list.size());

    bool first_occurance_found = false;

    for(int j=0;j<N;j++)
    {
        int tLabel = master_label3i_list[j];
        if(classId==tLabel)
        {
            ok = true;

            if(first_occurance_found==false)
            {
                first_occurance_found = true;
                start_indx=j;
            }
            stop_indx = j;
        }
    }

    return ok;
}

bool cClassifierData::remove_items_from_pt_list(int start_indx, int stop_indx)
{
    master_label_list.erase(master_label_list.begin() + start_indx, master_label_list.begin() + stop_indx);
    master_data_list.erase(master_data_list.begin() + start_indx, master_data_list.begin() + stop_indx);
}

bool cClassifierData::remove_items_from_pt3i_list(int start_indx, int stop_indx)
{
    master_label_list.erase(master_label3i_list.begin() + start_indx, master_label3i_list.begin() + stop_indx);
    master_data3i_list.erase(master_data3i_list.begin() + start_indx, master_data3i_list.begin() + stop_indx);
}

bool cClassifierData::remove_item_from_pt_list(int indx)
{
    master_label_list.erase(master_label_list.begin() + indx);
    master_data_list.erase(master_data_list.begin() + indx);
}

bool cClassifierData::remove_item_from_pt3i_list(int indx)
{
    master_label3i_list.erase(master_label3i_list.begin() + indx);
    master_data3i_list.erase(master_data3i_list.begin() + indx);
}

void cClassifierData::clear_class_from_list(int classId)
{
    int start_indx, stop_indx;

    bool found = find_first_last_indx_classId(classId, start_indx, stop_indx);

    if(found)
        remove_items_from_pt_list(start_indx, stop_indx);

}

bool cClassifierData::get_data_by_classId(int classId, vector<int> &label_list, vector<Point> &pt_data_list )
{
    bool ok = false;

    //first make sure lists are sorted
    sort_class_lists();

    int start_indx, stop_indx;

    //get start and stop index for this classId
    ok = find_first_last_indx_classId(classId, start_indx, stop_indx);

    if(ok)
    {
        //extract subVectors
        vector<int> classLabelList(master_label_list.begin() + start_indx, master_label_list.begin() + stop_indx);
        vector<Point> classPtList(master_data_list.begin() + start_indx, master_data_list.begin() + stop_indx);

        //make deep copies
        label_list = classLabelList;
        pt_data_list = classPtList;
    }

    return ok;
}

bool cClassifierData::get_data_by_3iclassId(int classId, vector<int> &label_list, vector<Point3i> &pt_data_list )
{
    bool ok = false;

    //first make sure lists are sorted
    sort_class_lists();

    int start_indx, stop_indx;

    //get start and stop index for this classId
    ok = find_first_last_3i_indx_classId(classId, start_indx, stop_indx);

    if(ok)
    {
        //extract subVectors
        vector<int> classLabelList(master_label3i_list.begin() + start_indx, master_label3i_list.begin() + stop_indx);
        vector<Point3i> classPtList(master_data3i_list.begin() + start_indx, master_data3i_list.begin() + stop_indx);

        //make deep copies
        label_list = classLabelList;
        pt_data_list = classPtList;
    }

    return ok;
}

void cClassifierData::append_testSet(vector<Point> &newTrainData, vector<int> &newTrainLabels, vector<Point> &trainData, vector<int> &trainLabels)
{

    trainLabels.insert( trainLabels.end(), newTrainLabels.begin(), newTrainLabels.end() );
    trainData.insert( trainData.end(), newTrainData.begin(), newTrainData.end() );

}

void cClassifierData::append_3i_testSet(vector<Point3i> &newTrainData3i, vector<int> &newTrainLabels3i, vector<Point3i> &trainData3i, vector<int> &trainLabels3i)
{
    trainLabels3i.insert( trainLabels3i.end(), newTrainLabels3i.begin(), newTrainLabels3i.end() );
    trainData3i.insert( trainData3i.end(), newTrainData3i.begin(), newTrainData3i.end() );
}

bool cClassifierData::get_random_trainingTestSet(int classId, vector<Point> &trainData, vector<int> &trainLabels, vector<Point> &testData, vector<int> &testLabels)
{
    bool ok = false;

    //create sequential vector

    int start_indx, stop_indx;
    int N = stop_indx - start_indx + 1;

    //get start and stop index for this classId
    ok = find_first_last_indx_classId(classId, start_indx, stop_indx);

    vector<int> indx_list(N);
    int i = start_indx;
    std::fill(indx_list.begin(), indx_list.end(), i++);

    //shuffle index vector
    std::random_shuffle(indx_list.begin(), indx_list.end());


    vector<int> trainLabelList, testLabelList;
    vector<Point> trainPtList, testPtList;

    int splitPoint = N/2;
    for(int i=0;i<N;i++)
    {
        if(i<splitPoint)
        {
            int indx = indx_list[i];
            int label = master_label_list[indx];
            Point pt = master_data_list[indx];
            trainLabelList.push_back(label);
            trainPtList.push_back(pt);
        }
        else
        {
            int indx = indx_list[i];
            int label = master_label_list[indx];
            Point pt = master_data_list[indx];
            testLabelList.push_back(label);
            testPtList.push_back(pt);
        }
    }

    //make deep copies of all lists
    trainData = trainPtList;
    trainLabels = trainLabelList;
    testData = testPtList;
    testLabels = testLabelList;

    return ok;
}

bool cClassifierData::get_random_3i_trainingTestSet(int classId, vector<Point3i> &trainData3i, vector<int> &trainLabels3i, vector<Point3i> &testData3i, vector<int> &testLabels3i)
{
    bool ok = false;

    //create sequential vector

    int start_indx, stop_indx;

    //get start and stop index for this classId
    ok = find_first_last_3i_indx_classId(classId, start_indx, stop_indx);


    int N = stop_indx - start_indx + 1;

    if(N <= 0)
    {
        return ok;
    }

    vector<int> indx_list(N);
    int i = start_indx;
    std::fill(indx_list.begin(), indx_list.end(), i++);

    //shuffle index vector
    std::random_shuffle(indx_list.begin(), indx_list.end());


    vector<int> trainLabelList, testLabelList;
    vector<Point3i> trainPtList, testPtList;

    int splitPoint = N/2;
    for(int i=0;i<N;i++)
    {
        if(i<splitPoint)
        {
            int indx = indx_list[i];
            int label = master_label3i_list[indx];
            Point3i pt = master_data3i_list[indx];
            trainLabelList.push_back(label);
            trainPtList.push_back(pt);
        }
        else
        {
            int indx = indx_list[i];
            int label = master_label3i_list[indx];
            Point3i pt = master_data3i_list[indx];
            testLabelList.push_back(label);
            testPtList.push_back(pt);
        }
    }

    //make deep copies of all lists
    trainData3i = trainPtList;
    trainLabels3i = trainLabelList;
    testData3i = testPtList;
    testLabels3i = testLabelList;

    return ok;
}


bool cClassifierData::get_classId_list(vector<int> &classId_list)
{
    bool ok = false;

    vector<int> cId_list = master_label_list;

    sort_class_lists();

    cId_list.erase( unique( cId_list.begin(), cId_list.end() ), cId_list.end() );

    classId_list = cId_list;

    return ok;
}

bool cClassifierData::get_classId_3i_list(vector<int> &classId_list)
{
    bool ok = false;

    vector<int> cId_list = master_label3i_list;

    sort_class_3iLists();

    cId_list.erase( unique( cId_list.begin(), cId_list.end() ), cId_list.end() );

    classId_list = cId_list;

    return ok;
}

bool cClassifierData::get_random_trainingTestSet(vector<Point> &trainData, vector<int> &trainLabels, vector<Point> &testData, vector<int> &testLabels)
{
    bool ok = false;

    vector<int> classId_list;
    get_classId_list(classId_list);

    int N = static_cast<int>(classId_list.size());

    for(int i=0;i<N;i++)
    {
        vector<Point> newTrainData;
        vector<int> newTrainLabels;
        vector<Point> newTestData;
        vector<int> newTestLabels;

        int classId = classId_list[i];

        bool ok = get_random_trainingTestSet(classId, newTrainData,  newTrainLabels,  newTestData, newTestLabels);

        append_testSet(newTrainData, newTrainLabels, trainData, trainLabels);
        append_testSet(newTestData, newTestLabels, testData, testLabels);

    }

    return ok;
}

bool cClassifierData::get_random_3i_trainingTestSet(vector<Point3i> &trainData3i, vector<int> &trainLabels3i, vector<Point3i> &testData3i, vector<int> &testLabels3i)
{
    bool ok = false;

    vector<int> classId_list;
    get_classId_3i_list(classId_list);

    int N = static_cast<int>(classId_list.size());

    for(int i=0;i<N;i++)
    {
        vector<Point3i> newTrainData3i;
        vector<int> newTrainLabels3i;
        vector<Point3i> newTestData3i;
        vector<int> newTestLabels3i;

        int classId = classId_list[i];

        bool ok = get_random_3i_trainingTestSet(classId, newTrainData3i,  newTrainLabels3i,  newTestData3i, newTestLabels3i);

        append_3i_testSet(newTrainData3i, newTrainLabels3i, trainData3i, trainLabels3i);
        append_3i_testSet(newTestData3i, newTestLabels3i, testData3i, testLabels3i);

    }

    return ok;
}


bool cClassifierData::save_current_trainingData_to_csv(QString filePathName, QString headerLine, bool append)
{
    cOpenCv_Utils cocv;
    cUtils cutls;

    sort_class_lists();

    int N = static_cast<int>(master_label_list.size());

    QList<QStringList> csv_list;


    for(int i=0;i<N;i++)
    {
        int labelId = master_label_list[i];
        QString sLabelId = QString::number(labelId);
        Point pt = master_data_list[i];

        QString sPt = cocv.convert_point_to_csv_QString(pt);

        QStringList strList;

        strList.append(sLabelId);
        strList.append(sPt);

        csv_list.append(strList);
    }

    bool ok = cutls.save_csv_file(csv_list, filePathName, headerLine, append);

    return ok;
}

bool cClassifierData::save_current_3i_trainingData_to_csv(QString filePathName, QString headerLine, bool append)
{
    cOpenCv_Utils cocv;
    cUtils cutls;

    sort_class_3iLists();

    int N = static_cast<int>(master_label3i_list.size());

    QList<QStringList> csv_list;


    for(int i=0;i<N;i++)
    {
        int labelId = master_label3i_list[i];
        QString sLabelId = QString::number(labelId);
        Point3i hsv_pt = master_data3i_list[i];
        Point3i bgr_pt = master_bgr_data3i_list[i];

        QString sHsvPt = cocv.convert_point3i_to_csv_QString(hsv_pt);
        QString sBgrPt = cocv.convert_point3i_to_csv_QString(bgr_pt);

        QStringList strList;

        strList.append(sLabelId);
        strList.append(sBgrPt);
        strList.append(sHsvPt);

        csv_list.append(strList);
    }

    bool ok = cutls.save_csv_file(csv_list, filePathName, headerLine, append);

    return ok;
}


//assumes data as written above in save_current_trainingData_to_csv() {"labelId,B,G,R,H,S,V"}
bool cClassifierData::read_csv_trainingData(QString filePathName, vector<Point> &trainData, vector<int> &trainLabels)
{
    cUtils cutls;

    bool ok = false;

    QList<QStringList> list_strLists = cutls.read_csv_file(filePathName);



    int N = list_strLists.length();

    //start at 1 to skip headerLine
    for(int i=1;i<N;i++)
    {
        QStringList strList = list_strLists[i];

        //each string should be unique entry
        int M = strList.length();

        if(M>=3)
        {
            QString sLabel = strList[0];
            QString sB = strList[1];
            QString sG = strList[1];
            QString sR = strList[1];

            int label = sLabel.toInt();
            int B = sB.toInt();
            int G = sG.toInt();
            int R = sR.toInt();

            Point pt;
            pt.x = B;
            pt.y = G;

            trainLabels.push_back(label);
            trainData.push_back(pt);
        }
        ok = true;
    }


    return ok;
}

//assumes data as written above in save_current_3i_trainingData_to_csv() {"labelId,R,G,B"}
bool cClassifierData::read_3i_csv_trainingData(QString filePathName, vector<Point3i> &bgr_trainData, vector<Point3i> &hsv_trainData, vector<int> &trainLabels)
{
    cUtils cutls;

    bool ok = false;

    QList<QStringList> list_strLists = cutls.read_csv_file(filePathName);

    int N = list_strLists.length();

    //start at 1 to skip headerLine
    for(int i=1;i<N;i++)
    {
        QStringList strList = list_strLists[i];

        //each string should be unique entry
        int M = strList.length();

        if(M>=7)
        {
            QString sLabel = strList[0];
            QString sB = strList[1];
            QString sG = strList[2];
            QString sR = strList[3];
            QString sH = strList[4];
            QString sS = strList[5];
            QString sV = strList[6];


            int label = sLabel.toInt();
            int B = sB.toInt();
            int G = sG.toInt();
            int R = sR.toInt();
            int H = sH.toInt();
            int S = sS.toInt();
            int V = sV.toInt();

            Point3i bgr_pt, hsv_pt;
            bgr_pt.x = B;
            bgr_pt.y = G;
            bgr_pt.z = R;
            hsv_pt.x = H;
            hsv_pt.y = S;
            hsv_pt.z = V;


            trainLabels.push_back(label);
            bgr_trainData.push_back(bgr_pt);
            hsv_trainData.push_back(hsv_pt);
        }
        else
        {
            ok = false;
            return ok;
        }
        ok = true;
    }


    return ok;
}

bool cClassifierData::load_trainingData(vector<Point> &trainData, vector<int> &trainLabels)
{
    bool ok = false;

    int n = static_cast<int>(trainData.size());
    int m = static_cast<int>(trainLabels.size());

    if(n==m)
    {
        ok = true;
        clear_all();

        master_data_list = trainData;
        master_label_list = trainLabels;
    }

}

bool cClassifierData::load_trainingData(vector<Point3i> &bgr_trainData, vector<Point3i> &hsv_trainData, vector<int> &trainLabels)
{
    bool ok = false;

    int k = static_cast<int>(bgr_trainData.size());
    int n = static_cast<int>(hsv_trainData.size());
    int m = static_cast<int>(trainLabels.size());

    if((n==m)&(k==m))
    {
        ok = true;
        clear_all();

        master_bgr_data3i_list = bgr_trainData;
        master_data3i_list = hsv_trainData;
        master_label3i_list = trainLabels;
    }

    return ok;
}


bool cClassifierData::is_pixelList_biModal(vector<Point> pt_list)
{
    bool ok = false;



    return ok;
}

bool cClassifierData::is_pixelList_biModal(vector<Point3i> pt_list)
{
    bool ok = false;



    return ok;
}

bool cClassifierData::split_biModal(vector<Point> pt_list, vector<Point> &pt_list_a, vector<Point> &pt_list_b)
{
    bool ok = false;



    return ok;
}

bool cClassifierData::split_biModal(vector<Point3i> pt_list, vector<Point3i> &pt_list_a, vector<Point3i> &pt_list_b)
{
    bool ok = false;



    return ok;
}

