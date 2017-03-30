#include "cutils.h"



cUtils::cUtils(QObject *parent) : QObject(parent)
{

}

/*
int cUtils::SizeType_toInt(size_type data)
{
    if (data > std::numeric_limits<int>::max())
        throw std::exception("Invalid cast.");

    int n = static_cast<int>(data);

    return n;
}
*/

int cUtils::SizeT_toInt(size_t data)
{

    int n = static_cast<int>(data);

    return n;
}


std::string cUtils::convert_QString_to_stdString(QString qstr)
{
    //note: works for all except windows
    std::string sstr= qstr.toUtf8().constData();

    //on windows do this
    //std::string sstr = qstr.toLocal8Bit().constData();

    return sstr;
}


QString cUtils::get_openFileName()
{
    QString filePathName = QFileDialog::getOpenFileName(0,
         tr("Open File"), "/home", tr("All Files (*.*)"));

    return filePathName;

}

QString cUtils::get_openFileName(QString fileExt)
{
    QString searchFilter = "File: (*." + fileExt +");All Files (*.*)";

    QString filePathName = QFileDialog::getOpenFileName(
                0,
                tr("Open File"),
                QString("/home"),
                tr("All Files (*.*)") );

    return filePathName;

}

QString cUtils::get_openFileName(QString filePathName, QString fileExt)
{
    QString searchFilter = "File: (*." + fileExt +");All Files (*.*)";

    filePathName = QFileDialog::getOpenFileName(
                0,
                tr("Open File"),
                QString("/home"),
                tr("All Files (*.*)") );

    return filePathName;
}

QString cUtils::get_saveFileName(QString filePathName, QString fileExt)
{
    QString searchFilter = "File: (*." + fileExt +");All Files (*.*)";

    filePathName = QFileDialog::getSaveFileName(
            0,  //must be null as we're not using a Window widget here
            QString("Save Image As:"),
            QString("/home/"),
            "Image File: (*.jpg); All files (*.*)" );


    return filePathName;
}

QString cUtils::get_saveFileName(QString fileExt)
{
    QString searchFilter = "File: (*." + fileExt +");All Files (*.*)";

    QString filePathName = QFileDialog::getSaveFileName(
            0,  //must be null as we're not using a Window widget here
            QString("Save Image As:"),
            QString("/home/"),
            "Image File: (*.jpg); All files (*.*)" );


    return filePathName;
}

QString cUtils::get_saveFileName()
{

    QString filePathName = QFileDialog::getSaveFileName(
            0,  //must be null as we're not using a Window widget here
            QString("Save Image As:"),
            QString("/home/"),
            "Image File: (*.jpg); All files (*.*)" );


//    QString filePathName = "/home/cfd0/Documents/training_set.csv";


    return filePathName;

}

bool cUtils::save_csv_file(QList<QStringList> csv_list, QString filePathName, QString headerLine, bool append)
{
    bool ok = false;

    QFile data(filePathName);

    bool dataFileIsOpen;

    if(append)
    {
        dataFileIsOpen = data.open(QFile::WriteOnly|QFile::Append);
        if(!dataFileIsOpen)
        {
            dataFileIsOpen = data.open(QFile::WriteOnly|QFile::Truncate);

        }
    }
    else
    {
        dataFileIsOpen = data.open(QFile::WriteOnly|QFile::Truncate);
    }

    if(dataFileIsOpen)
    {
        QTextStream output(&data);

        //write first line
        output << headerLine << "\n";

        //write the csv lines from list
        int N = csv_list.length();

        for(int i=0;i<N;i++)
        {
            int M = csv_list[i].length();
            for(int j=0;j<M;j++)
            {
                QStringList slist = csv_list[i];
                QString sCsvStr = slist[j];

                if(j==(M-1))
                {
                    output << sCsvStr << "\n";
                }
                else
                {
                    output << sCsvStr << ",";
                }
            }
        }


        data.close();

        ok = true;
    }

    return ok;
}

QList<QStringList> cUtils::read_csv_file(QString filePathName)
{
    QStringList sLineList;
    QList<QStringList> csvLines;
    QByteArray line;

    QFile file(filePathName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << file.errorString();
        return csvLines;
    }


    if(!file.atEnd())
    {
        line = file.readLine();
    }

    while (!file.atEnd())
    {
        line = file.readLine();

        QList<QByteArray> blist = line.split(',');

        int N = blist.length();

        if(N <=0 )
        {
            break;
        }

        sLineList.clear();

        for(int i=0;i<N;i++)
        {

            QString str = blist[i].constData();
            sLineList.append(str);
        }
        qDebug() << sLineList;

        csvLines.append(sLineList);

    }

    return csvLines;
}


