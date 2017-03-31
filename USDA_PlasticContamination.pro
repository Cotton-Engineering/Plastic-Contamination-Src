#-------------------------------------------------
#
# Project created by QtCreator 2017-02-11T02:54:27
#
# Updated 3-29-17
#-------------------------------------------------

QT       += core gui opengl
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = USDA_PlasticContamination
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    dialog_create_classifier.cpp \
    cclassifier.cpp \
    my_qlabel.cpp \
    copencv_utils.cpp \
    cclassifierdata.cpp \
    cutils.cpp

HEADERS  += mainwindow.h \
    dialog_create_classifier.h \
    cclassifier.h \
    my_qlabel.h \
    copencv_utils.h \
    cclassifierdata.h \
    cutils.h

FORMS    += mainwindow.ui \
    dialog_create_classifier.ui


INCLUDEPATH += .
INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/opencv2
INCLUDEPATH += /usr/local/include/opencv2/core
INCLUDEPATH += /usr/local/include/opencv2/highgui
INCLUDEPATH += /usr/local/include/opencv2/imgproc
INCLUDEPATH += /usr/local/include/opencv2/flann
INCLUDEPATH += /usr/local/include/opencv2/photo
INCLUDEPATH += /usr/local/include/opencv2/video
INCLUDEPATH += /usr/local/include/opencv2/features2d
INCLUDEPATH += /usr/local/include/opencv2/objdetect
INCLUDEPATH += /usr/local/include/opencv2/calib3d
INCLUDEPATH += /usr/local/include/opencv2/ml
INCLUDEPATH += /usr/local/include/opencv2/contrib

#add includes for flycapture cameras
INCLUDEPATH += /usr/include


#INCLUDEPATH += /usr/lib/x86_64-linux-gnu/
#INCLUDEPATH += /ect/udev
#INCLUDEPATH += /lib/udev



# Set OpenCV library paths
# LIBS += `pkg-config opencv --cflags --libs`
LIBS += -L/usr/local/lib -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_viz -lopencv_core

#add libs for flycapture cameras
LIBS += -L/usr/lib
//LIBS += -lflycapture

DISTFILES +=

RESOURCES += \
    config.qrc

