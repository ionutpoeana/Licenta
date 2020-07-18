#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widgets/violationwidget.h"

#include "Licenta_all_include.gen.h"

#include <QSqlDatabase>
#include <QAction>
#include <QtDebug>
#include <QPixmap>

long long TIME;

int DISTANCE_THRESHOLD;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_database = DatabaseFactory::getConnection();
    m_saveVideoThread = new SaveVideoThread(m_database,nullptr);
    TIME = 0;

    m_violationsDetails = new CameraViolationDetailsWidget(ui->gbCameraViolations);


    bool isSignalSlotConnected = connect(m_saveVideoThread,&SaveVideoThread::violationSavedSignal,this,&MainWindow::receiveViolationSlot);
    if(isSignalSlotConnected)
    {
        qDebug()<<m_saveVideoThread->metaObject()->className()<<" violationSavedSignal connection with " <<this->metaObject()->className() << " SUCCEDED!" ;
    }
    else
    {
        qDebug()<<this->metaObject()->className()<<" sendCurrentFrameSignal connection with " <<m_cameraStream->metaObject()->className() << " FAILED!" ;
    }

    connect(ui->actionAdd_camera, &QAction::triggered,this,&MainWindow::on_actionAddCamera);
    m_addCameraWidget = nullptr;


    m_mutex.lock();
    qx::dao::fetch_all(m_cameraList,&m_database);
    m_mutex.unlock();

    loadCameraLocations();

    connect(ui->cbCameraLocation, SIGNAL(activated(int)),this, SLOT(on_cbCameraLocationIndexChanged(int)));
    connect(ui->cbName, SIGNAL(activated(int)),this,SLOT(on_cbNameIndexChanged(int)));
}

MainWindow::~MainWindow()
{
    if(m_addCameraWidget!=nullptr)
        delete m_addCameraWidget;

    if(m_cameraStream!=nullptr)
        delete m_cameraStream;

    if(m_saveVideoThread!=nullptr)
        delete m_saveVideoThread;

    if(m_violationsDetails!=nullptr)
        delete m_violationsDetails;

    delete ui;
    ui= nullptr;
}

void MainWindow::receiveCameraStreamSlot(CameraStream *cameraStream)
{
    if(m_addCameraWidget!=nullptr)
    {
        delete m_addCameraWidget;
        m_addCameraWidget = nullptr;
    }

    if(m_cameraStream!=nullptr)
    {
        delete m_cameraStream;
    }

    m_cameraStream = cameraStream;
    Camera camera;
    camera.setName(cameraStream->m_camera.data()->getName());
    camera.setCameraId(cameraStream->m_camera.data()->getCameraId());
    camera.setLocation(cameraStream->m_camera.data()->getLocation());
    camera.setStreamLocation(cameraStream->m_camera.data()->getStreamLocation());
    m_cameraList.push_back(camera);

    loadCameraNames(m_cameraStream->m_camera->getLocation());
    loadCameraViolations(m_cameraStream->m_camera->getCameraId());
    ui->cbName->setCurrentText(m_cameraStream->m_camera->getName());

    if(ui->cbCameraLocation->findText(camera.getLocation())==-1)
    {
        ui->cbCameraLocation->addItem(camera.getLocation());
    }
    ui->cbCameraLocation->setCurrentText(camera.getLocation());


    bool isSignalSlotConnected = connect(m_cameraStream, &CameraStream::violationDetectedSignal, m_saveVideoThread, &SaveVideoThread::saveViolationSlot);
    if(isSignalSlotConnected)
    {
        qDebug()<<m_cameraStream->metaObject()->className()<<" violationDetectedSignal connection with " <<m_saveVideoThread->metaObject()->className() << " SUCCEDED!" ;
    }
    else
    {
        qDebug()<<this->metaObject()->className()<<" violationDetectedSignal connection with " <<m_saveVideoThread->metaObject()->className() << " FAILED!" ;
    }

    connect(this,&MainWindow::displayStreamSignal, m_cameraStream, &CameraStream::displayStreamSlot);
    emit displayStreamSignal(m_cameraStream->m_camera->getCameraId());
    isSignalSlotConnected =   connect(m_cameraStream,&CameraStream::sendCurrentFrameSignal, this, &MainWindow::receiveFrameSlot);
    if(isSignalSlotConnected)
    {
        qDebug()<<this->metaObject()->className()<<" sendCurrentFrameSignal connection with " <<m_cameraStream->metaObject()->className() << " SUCCEDED!" ;
    }
    else
    {
        qDebug()<<this->metaObject()->className()<<" sendCurrentFrameSignal connection with " <<m_cameraStream->metaObject()->className() << " FAILED!" ;
    }

    m_cameraStream->startStreamProcessing();
}

void MainWindow::receiveViolationSlot(Violation *violation)
{
    for(const auto&camera : m_cameraList)
    {
        if(camera.getName() == ui->cbName->currentText()
                && camera.getLocation() == ui->cbCameraLocation->currentText()
                && violation->getCamera()->getCameraId() == camera.getCameraId())
        {
            m_violationsDetails->addViolationItem(*violation);
            break;
        }
    }
}

void MainWindow::receiveFrameSlot(QImage *image)
{
    QImage scaldedImgge = image->scaled(ui->lblVideo->size(),Qt::KeepAspectRatio);
    ui->lblVideo->setPixmap(QPixmap::fromImage(*image));
    TIME+=NORMAL_FRAME_TIME;
}

void MainWindow::receiveStreamErrorSlot(QUuid streamId, QString error)
{

}

void MainWindow::receiveFinishedStreamSlot(QUuid streamId)
{

}


void MainWindow::on_actionAddCamera()
{
    if(m_addCameraWidget!=nullptr)
    {
        delete m_addCameraWidget;
    }
    m_addCameraWidget = new AddCameraWidget(&m_database);

    bool isSignalSlotConnected = connect(m_addCameraWidget,&AddCameraWidget::cameraSavedSignal, this, &MainWindow::receiveCameraStreamSlot);
    if(isSignalSlotConnected)
    {
        qDebug()<<this->metaObject()->className()<<" connection with " <<m_addCameraWidget->metaObject()->className() << " SUCCEDED!" ;
    }
    else
    {
        qDebug()<<this->metaObject()->className()<<" connection with " <<m_addCameraWidget->metaObject()->className() << " FAILED!" ;
    }
}

void MainWindow::on_cbNameIndexChanged(int index)
{

    if(m_cbNameLastIndex !=index)
    {
        for(const auto&camera : m_cameraList)
        {
            if(ui->cbName->itemText(index).remove(' ') == camera.getName().remove(' '))
            {
                loadCameraViolations(camera.getCameraId());
            }
        }
        m_cbNameLastIndex = index;
    }
}

void MainWindow::on_cbCameraLocationIndexChanged(int index)
{
    loadCameraNames(ui->cbCameraLocation->itemText(index).remove(' '));
}

void MainWindow::loadCameraViolations(const QUuid& cameraId)
{

    m_violationsList.clear();
    delete m_violationsDetails;
    m_violationsDetails = new CameraViolationDetailsWidget(ui->gbCameraViolations);

    qx_query query;
    query.where("t_violation.camera").isEqualTo(cameraId.toString());
    qx::dao::fetch_by_query(query,m_violationsList,&m_database);
    m_violationsDetails->addViolationItems(m_violationsList);
}

void MainWindow::loadCameraLocations()
{
    if(m_cameraList.isEmpty())
    {
        qDebug()<<this->metaObject()->className() <<"\tNo cameras in database!";
        return;
    }

    Camera * currentCamera = nullptr;
    for(auto&camera: m_cameraList)
    {
        if(ui->cbCameraLocation->findText(camera.getLocation())==-1)
        {
            ui->cbCameraLocation->addItem(camera.getLocation());
        }

        if(ui->cbCameraLocation->currentText() == camera.getLocation())
        {
            currentCamera = &camera;
        }

    }

    loadCameraNames(currentCamera->getLocation());
    loadCameraViolations(currentCamera->getCameraId());
}

void MainWindow::loadCameraNames(const QString &location)
{

    if(ui->cbName->count()!=0)
    {
        ui->cbName->clear();
    }

    for(const auto&camera : m_cameraList)
    {
        if(camera.getLocation().remove(' ')==QString(location).remove(' '))
        {
            ui->cbName->addItem(camera.getName());
        }
    }
}
