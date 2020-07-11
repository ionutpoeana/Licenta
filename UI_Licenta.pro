QT       += core gui multimedia multimediawidgets widgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ocv/camerastream.cpp \
    ocv/component.cpp \
    ocv/proof.cpp \
    ocv/rule.cpp \
    ocv/semaphore.cpp \
    ocv/utils.cpp\
    processvideothread.cpp \
    savevideothread.cpp \
    widgets/addcamerawidget.cpp \
    widgets/cameraviolationdetailswidget.cpp \
    widgets/violationwidget.cpp



HEADERS += \
    mainwindow.h \
    ocv/camerastream.h \
    ocv/component.h \
    ocv/constants.h \
    ocv/proof.h \
    ocv/rule.h \
    ocv/semaphore.h \
    ocv/ utils.h\
    processvideothread.h \
    savevideothread.h \
    widgets/addcamerawidget.h \
    widgets/cameraviolationdetailswidget.h \
    widgets/qtConstants.h \
    widgets/violationwidget.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += C:\Qt\OpenCV\install\include
INCLUDEPATH += C:\Qt\QxOrm\include
INCLUDEPATH += C:\Users\ionut\Documents\UI_Licenta\QxOrm\include


LIBS += C:\Qt\OpenCV\install\x86\mingw\bin\libopencv_highgui410.dll
LIBS += C:\Qt\OpenCV\install\x86\mingw\bin\libopencv_core410.dll
LIBS += C:\Qt\OpenCV\install\x86\mingw\bin\libopencv_imgcodecs410.dll
LIBS += C:\Qt\OpenCV\install\x86\mingw\bin\libopencv_imgproc410.dll
LIBS += C:\Qt\OpenCV\install\x86\mingw\bin\libopencv_features2d410.dll
LIBS += C:\Qt\OpenCV\install\x86\mingw\bin\libopencv_calib3d410.dll
LIBS += C:\Qt\OpenCV\install\x86\mingw\bin\libopencv_videoio410.dll
LIBS += C:\Qt\OpenCV\install\x86\mingw\bin\libopencv_video410.dll
LIBS += C:\Qt\Tools\QtCreator\bin\plugins\sqldrivers\qsqlodbc.dll

LIBS += C:\Users\ionut\Documents\UI_Licenta\QxOrm\bin\Licenta.dll
LIBS += C:\Qt\QxOrm\lib\QxOrm.dll


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
