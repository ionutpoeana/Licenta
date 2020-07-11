#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ocv/utils.h"
#include "ocv/semaphore.h"
#include "ocv/proof.h"

#include "savevideothread.h"
#include "widgets/cameraviolationdetailswidget.h"
#include "widgets/addcamerawidget.h"

#include <QMainWindow>
#include <QVideoFrame>
#include <QTimer>
#include <QPixmap>
#include <QElapsedTimer>
#include <QSqlDatabase>
#include <QtSql>
#include <QtDebug>


#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void receiveCameraStream(CameraStream *cameraStream);
    void readFrame();
    void receiveViolation(const Violation& v);

    void on_actionAddCamera();

private:

    Ui::MainWindow *ui;

    const QString carCountingUnderBridge = "C:\\Users\\ionut\\Desktop\\Licenta\\video\\CarsDrivingUnderBridge.mp4";

    CameraViolationDetailsWidget* m_violationsDetails;
    AddCameraWidget* m_addCameraWidget = nullptr;

    QTimer* m_displayVideoTimer;
    bool m_hasCrimeBeenCommitted;
    SaveVideoThread* m_saveVideoThread;
    QSqlDatabase m_database;

    CameraStream* m_cameraStream;
    cv::VideoCapture capture;
    cv::Mat frameMat;
    cv::Mat previousFrameMat;
    queue<Mat> proofOfCrime;
    int violationNumber;

};
#endif // MAINWINDOW_H
