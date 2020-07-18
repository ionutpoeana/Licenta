#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ocv/utils.h"
#include "ocv/semaphore.h"
#include "ocv/violationproof.h"

#include "savevideothread.h"
#include "widgets/cameraviolationdetailswidget.h"
#include "widgets/addcamerawidget.h"

#include "databasefactory.h"

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

    void receiveCameraStreamSlot(CameraStream *cameraStream);
    void receiveViolationSlot(Violation*violation);
    void receiveFrameSlot(QImage* image);
    void receiveStreamErrorSlot(QUuid streamId, QString error);
    void receiveFinishedStreamSlot(QUuid streamId);

    void on_actionAddCamera();
    void on_cbNameIndexChanged(int index);
    void on_cbCameraLocationIndexChanged(int index);
signals:
    void displayStreamSignal(QUuid steamId);
private:
    int m_cbNameLastIndex = 0;

    Ui::MainWindow *ui;

    CameraViolationDetailsWidget* m_violationsDetails;
    AddCameraWidget* m_addCameraWidget;

    SaveVideoThread* m_saveVideoThread;
    QSqlDatabase m_database;
    CameraStream* m_cameraStream = nullptr;

    QList<Camera> m_cameraList;
    QList<Violation> m_violationsList;
    QMutex m_mutex;

    void loadCameraLocations();
    void loadCameraNames(const QString& location);
    void loadCameraViolations(const QUuid& cameraId);
};
#endif // MAINWINDOW_H
