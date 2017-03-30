#ifndef CUTILS_H
#define CUTILS_H

#include <QObject>

#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QSettings>

#include <QMessageBox>
#include <QDesktopServices>
#include <QStandardPaths>

#include <QString>
#include <QStringList>
#include <QList>
#include <QByteArray>


#include <QDebug>


using namespace std;


class cUtils : public QObject
{
    Q_OBJECT
public:
    explicit cUtils(QObject *parent = 0);

signals:

public slots:

public:


    QList<QStringList> read_csv_file(QString filePathName);
    bool save_csv_file(QList<QStringList> csv_list, QString filePathName, QString headerLine, bool append=false);

    QString get_saveFileName();
    QString get_saveFileName(QString fileExt);
    QString get_saveFileName(QString filePathName, QString fileExt);

    QString get_openFileName();
    QString get_openFileName(QString fileExt);
    QString get_openFileName(QString filePathName, QString fileExt);

    std::string convert_QString_to_stdString(QString qstr);

    int SizeT_toInt(size_t data);
    //int SizeType_toInt(size_type data);

private:

};

#endif // CUTILS_H
