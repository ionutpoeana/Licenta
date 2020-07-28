#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ocv/utils.h"
#include "ocv/semaphore.h"
#include "ocv/violationproof.h"

#include "processstreamthread.h"
#include "savevideothread.h"
#include "widgets/cameraviolationdetailswidget.h"
#include "widgets/addcamerawidget.h"
#include "widgets/violationchartwidget.h"

#include "databasefactory.h"

#include <QMainWindow>
#include <QVideoFrame>
#include <QTimer>
#include <QPixmap>
#include <QElapsedTimer>
#include <QSqlDatabase>
#include <QtSql>
#include <QtDebug>
#include <QtCharts/QBarSeries>
#include <QStackedLayout>

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

private:
    int m_cbNameLastIndex = -1;
    const int MAX_THREAD_NUMBER = 3;
    int m_freeThreads = MAX_THREAD_NUMBER;

    QTimer* m_addCameraTimer;
    QMap<QUuid,ProcessStreamThread*> m_threads;
    QList<ProcessStreamThread*> m_threadsStorage;

    Ui::MainWindow *ui;

    CameraViolationDetailsWidget* m_violationsDetails;
    AddCameraWidget* m_addCameraWidget;

    ViolationChartWidget* m_cameraChart = nullptr;
    ViolationChartWidget* m_allCameraChart = nullptr;
    QComboBox* m_cbChart = nullptr;

    SaveVideoThread* m_saveVideoThread;
    QSqlDatabase m_database;

    QList<Camera> m_cameraList;
    QList<Violation> m_violationsList;
    QMutex m_mutex;

    void loadCameraLocations();
    void loadCameraNames(const QString& location);
    void loadCameraViolations(const QUuid& cameraId);
    void assignThread(cv::CameraStream*cameraStream);
    QtCharts::QBarSeries* groupByHour();
    QtCharts::QBarSeries* groupByCamera();
    void writeXML(const Camera& camera, QString fileName);
    void updateChart();
    Camera getCurrentCamera();

private slots:

    void receiveCameraStreamSlot(cv::CameraStream *cameraStream);
    void receiveViolationSlot(Violation*violation);
    void receiveFrameSlot(QImage* image);
    void receiveStreamErrorSlot(QUuid streamId, QString error);
    void receiveFinishedStreamSlot(QUuid streamId);

    void on_addCameraTimeOut();

    void on_actionAddCamera();
    void on_cbNameIndexChanged(int index);
    void on_cbCameraLocationIndexChanged(int index);
    void on_tgsViewInterestElementsToggled(bool checked);
    void on_actionExportCurrentStreamViolations();
    void on_actionExportAllStreamsViolations();
    void on_ActionHelp();
    void on_tgsViewCameraGraphToggled(bool checked);
signals:
    void displayStreamSignal(QUuid steamId);
    void displayStreamInterestItems(QUuid streamId, bool idDisplayed);
    void stopOrStopStreamProcessingSignal(bool stopOrStartStreamProcessing);


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

};
#endif // MAINWINDOW_H
