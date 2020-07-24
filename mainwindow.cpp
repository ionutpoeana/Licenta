#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widgets/violationwidget.h"

#include "Licenta_all_include.gen.h"

#include <camerastreamwrapper.h>

#include <QSqlDatabase>
#include <QAction>
#include <QtDebug>
#include <QPixmap>
#include <QMessageBox>
#include <QFileDialog>
#include <QHelpEngineCore>



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

    connect(m_saveVideoThread,&SaveVideoThread::violationSavedSignal,this,&MainWindow::receiveViolationSlot);
    connect(ui->actionAdd_camera, &QAction::triggered,this,&MainWindow::on_actionAddCamera);
    connect(ui->actionCurrent_stream,&QAction::triggered,this,&MainWindow::on_actionExportCurrentStreamViolations);
    connect(ui->actionAll_streams,&QAction::triggered, this, &MainWindow::on_actionExportAllStreamsViolations);
    connect(ui->actionHelp, &QAction::triggered, this, &MainWindow::on_ActionHelp);
    m_addCameraWidget = nullptr;


    m_mutex.lock();
    qx::dao::fetch_all(m_cameraList,&m_database);
    m_mutex.unlock();

    loadCameraLocations();

    connect(ui->cbCameraLocation, SIGNAL(activated(int)),this, SLOT(on_cbCameraLocationIndexChanged(int)));
    connect(ui->cbName, SIGNAL(activated(int)),this,SLOT(on_cbNameIndexChanged(int)));
    connect(ui->tgsViewInterestElements, SIGNAL(statusChanged(bool)),this,SLOT(on_tgsViewInterestElementsToggled(bool)));
    connect(ui->tgsViewCameraGraph, SIGNAL(statusChanged(bool)), this, SLOT(on_tgsViewCameraGraphToggled(bool)));

    for(int i = 0; i<MAX_THREAD_NUMBER; ++i)
    {
        m_threadsStorage.push_back(new ProcessStreamThread);
    }

    cv::Mat videoStreamEnded = cv::imread("C:\\Users\\ionut\\Documents\\build-UI_Licenta-Desktop_Qt_5_9_9_MinGW_32bit-Debug\\debug\\video stream ended.PNG");
    cvtColor(videoStreamEnded,videoStreamEnded,cv::COLOR_BGR2RGB);
    cv::resize(videoStreamEnded,videoStreamEnded,VIDEO_RESOLUTION);
    QImage image(videoStreamEnded.data,videoStreamEnded.cols, videoStreamEnded.rows, QImage::Format::Format_RGB888);
    ui->lblVideo->setPixmap(QPixmap::fromImage(image));
}

MainWindow::~MainWindow()
{


    for(int i =0; i<m_threadsStorage.size(); ++i)
    {
        if(m_threadsStorage[i]!=nullptr)
        {
            m_threadsStorage[i]->terminate();
            delete m_threadsStorage[i];
            m_threadsStorage[i] = nullptr;
        }
    }

    if(m_addCameraWidget!=nullptr)
        delete m_addCameraWidget;

    if(m_saveVideoThread!=nullptr)
        delete m_saveVideoThread;

    if(m_violationsDetails!=nullptr)
        delete m_violationsDetails;

    if(ui!=nullptr)
        delete ui;
    ui= nullptr;
}

void MainWindow::receiveCameraStreamSlot(cv::CameraStream *cameraStream)
{
    if(m_addCameraWidget!=nullptr)
    {
        delete m_addCameraWidget;
        m_addCameraWidget = nullptr;
    }

    emit stopStreamProcessingSignal(false);

    Camera camera;
    camera.setName(cameraStream->m_camera->getName());
    camera.setCameraId(cameraStream->m_camera->getCameraId());
    camera.setLocation(cameraStream->m_camera->getLocation());
    camera.setStreamLocation(cameraStream->m_camera->getStreamLocation());
    m_cameraList.push_back(camera);

    loadCameraNames(camera.getLocation());
    loadCameraViolations(camera.getCameraId());
    ui->cbName->setCurrentText(camera.getName());

    if(ui->cbCameraLocation->findText(camera.getLocation())==-1)
    {
        ui->cbCameraLocation->addItem(camera.getLocation());
    }
    ui->cbCameraLocation->setCurrentText(camera.getLocation());

    assignThread(cameraStream);
    m_threads[cameraStream->m_camera->getCameraId()]->start(QThread::Priority::HighPriority);
    qDebug("THREAD STARTED: Thred with id %d started for camera %s %s", m_threads[cameraStream->m_camera->getCameraId()]->getThreadId(),cameraStream->m_camera->getLocation().toStdString().c_str(), cameraStream->m_camera->getName().toStdString().c_str());

    ui->tgsViewInterestElements->setDown(false);
    emit displayStreamSignal(camera.getCameraId());

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

            if(ui->gbCameraViolationsGraph!=nullptr)
            {
                QtCharts::QBarSet *set = groupByHour();
                QtCharts::QBarSeries *series = new QtCharts::QBarSeries();
                series->append(set);
                m_violationChartWidget->removeChartSeries(series);
            }
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
    qDebug()<<"Error stream:\t"<<streamId<<" Message:"<<error;
}

void MainWindow::receiveFinishedStreamSlot(QUuid streamId)
{
    ProcessStreamThread* processThread = m_threads.find(streamId).value();

    qDebug("THREAD: Thread finished processing: threadId %d\tcameraId %s",processThread->getThreadId(),streamId.toString().toStdString().c_str());

    m_threads.remove(streamId);
    processThread->quit();
    ++m_freeThreads;
}

void MainWindow::on_actionAddCamera()
{
    if(m_freeThreads == 0)
    {
        QMessageBox mBox;
        mBox.setText("The concurent processing stream number is at limit.\nPlease wait for a stream to be finished!");
        mBox.exec();
        return;
    }


    if(m_addCameraWidget!=nullptr)
    {
        delete m_addCameraWidget;
    }

    emit stopStreamProcessingSignal(true);

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
    // rezolvat chestia aia cu multi threading
}

void MainWindow::on_cbNameIndexChanged(int index)
{
    for(const auto&thread:m_threadsStorage)
    {
        if(thread->isRunning())
        {
            qDebug("Thread %d is running", thread->getThreadId());
        }
    }

    if(m_cbNameLastIndex !=index)
    {
        for(const auto&camera : m_cameraList)
        {
            if(ui->cbName->itemText(index).remove(' ') == camera.getName().remove(' '))
            {
                if(m_threads.contains(camera.getCameraId()))
                {
                    emit displayStreamSignal(camera.getCameraId());
                }
                else
                {
                    emit displayStreamSignal(QUuid::createUuid());
                    cv::Mat videoStreamEnded = cv::imread("C:\\Users\\ionut\\Documents\\build-UI_Licenta-Desktop_Qt_5_9_9_MinGW_32bit-Debug\\debug\\video stream ended.PNG");
                    cvtColor(videoStreamEnded,videoStreamEnded,cv::COLOR_BGR2RGB);
                    cv::resize(videoStreamEnded,videoStreamEnded,VIDEO_RESOLUTION);
                    QImage image(videoStreamEnded.data,videoStreamEnded.cols, videoStreamEnded.rows, QImage::Format::Format_RGB888);
                    ui->lblVideo->setPixmap(QPixmap::fromImage(image));
                }

                loadCameraViolations(camera.getCameraId());

                if(ui->gbCameraViolationsGraph!=nullptr && ui->tgsViewCameraGraph->isEnabled())
                {
                    QtCharts::QBarSet *set = groupByHour();
                    QtCharts::QBarSeries *series = new QtCharts::QBarSeries();
                    series->append(set);
                    m_violationChartWidget->removeChartSeries(series);
                }
                break;
            }
        }
        m_cbNameLastIndex = index;
    }
}

void MainWindow::on_cbCameraLocationIndexChanged(int index)
{
    loadCameraNames(ui->cbCameraLocation->itemText(index).remove(' '));
}

void MainWindow::on_tgsViewInterestElementsToggled(bool checked)
{
    Camera camera = getCurrentCamera();
    if(m_threads.contains(camera.getCameraId()))
    {
        qDebug("Camera name: %s \tcameraId:%s \tchecked: %s",camera.getName().toStdString().c_str(),camera.getCameraId().toString().toStdString().c_str(),checked ==true?"true":"false");
        emit displayStreamInterestItems(camera.getCameraId(), checked);
    }
    else
    {
        ui->tgsViewInterestElements->setStatus(false);
    }
}

void MainWindow::on_actionExportCurrentStreamViolations()
{
    if(m_cameraList.size()>0)
    {
        Camera camera = getCurrentCamera();
        int ret = QMessageBox::Yes;
        if(m_violationsList.size() == 0)
        {
            QMessageBox msgB;
            msgB.setText("There are no violations for this camera! Do you still want to export?");
            msgB.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            ret = msgB.exec();
        }

        switch (ret)
        {
        case QMessageBox::Yes:
        {
            QString file = camera.getLocation().remove(' ').append('_').append(camera.getName().remove(' ')).append(".xml");
            QString fileName = QFileDialog::getSaveFileName(this,"Save violations!",file);
            writeXML(camera,fileName);
            break;
        }
        case QMessageBox::No:
            break;
        }
    }
    else
    {
        QMessageBox msgB;
        msgB.setText("There are no streams to export!");
        msgB.exec();
    }
}

void MainWindow::on_actionExportAllStreamsViolations()
{


    if(m_cameraList.size()>0)
    {

        QString fileName = QFileDialog::getSaveFileName(this,"Save violations!", "All_streams_violations.xml");

        QFile file(fileName);
        file.open(QIODevice::WriteOnly);

        QXmlStreamWriter xmlStream(&file);
        xmlStream.setAutoFormatting(true);
        xmlStream.writeStartDocument();
        xmlStream.writeStartElement("cameras");

        for(const auto&camera : m_cameraList)
        {
            xmlStream.writeStartElement("camera");
            QList<Violation> violations;
            qx_query query;
            m_mutex.lock();
            query.where("t_violation.camera").isEqualTo(camera.getCameraId().toString());
            qx::dao::fetch_by_query(query,violations,&m_database);
            m_mutex.unlock();

            xmlStream.writeTextElement("name",camera.getName().remove(' '));
            xmlStream.writeTextElement("location",camera.getLocation().remove(' '));
            xmlStream.writeTextElement("violationsNumber",QString(violations.size() + '0'));
            xmlStream.writeStartElement("violations");

            for(const auto&violation :violations)
            {
                xmlStream.writeStartElement("violation");
                xmlStream.writeTextElement("dateTime", violation.getTime().toString());
                xmlStream.writeTextElement("type", cv::enumToString((RULE_TYPE)violation.getRuleType()).c_str());
                xmlStream.writeEndElement();
            }
            xmlStream.writeEndElement();
            xmlStream.writeEndElement();

        }
        xmlStream.writeEndElement();
        xmlStream.writeEndDocument();

        file.close();

    }
    else
    {
        QMessageBox msgB;
        msgB.setText("There are no streams to export!");
        msgB.exec();
    }
}

void MainWindow::on_ActionHelp()
{

    QProcess::execute("hh.exe SurveilanceCamera.chm");
}

void MainWindow::on_tgsViewCameraGraphToggled(bool checked)
{
    if(checked)
    {
        if(ui->gbCameraViolationsGraph==nullptr)
        {
            QtCharts::QBarSet *set = groupByHour();
            QtCharts::QBarSeries *series = new QtCharts::QBarSeries();
            series->append(set);

            ui->gbCameraViolationsGraph= new QGroupBox(ui->gbCameraDetails);
            m_violationChartWidget = new ViolationChartWidget(series, ui->gbCameraViolationsGraph);
            m_violationChartWidget->move(5,5);
            ui->gbCameraViolationsGraph->setFixedSize(330,240);
            ui->gbCameraViolationsGraph->move(5,85);

        }

        ui->gbCameraViolations->move(5,330);
        ui->gbCameraViolations->resize(330,330);
        ui->gbCameraViolations->repaint();
        ui->gbCameraViolationsGraph->show();

    }
    else
    {
        ui->gbCameraViolationsGraph->hide();
        ui->gbCameraViolations->resize(330,630);
        m_violationsDetails->resize(330,600);
        ui->gbCameraViolations->move(5,85);
        m_violationsDetails->repaint();

    }

}

void MainWindow::loadCameraViolations(const QUuid& cameraId)
{

    m_violationsList.clear();
    delete m_violationsDetails;
    m_violationsDetails = new CameraViolationDetailsWidget(ui->gbCameraViolations);

    qx_query query;
    m_mutex.lock();
    query.where("t_violation.camera").isEqualTo(cameraId.toString());
    qx::dao::fetch_by_query(query,m_violationsList,&m_database);
    m_mutex.unlock();
    m_violationsDetails->addViolationItems(m_violationsList);
}

void MainWindow::assignThread(cv::CameraStream *cameraStream)
{
    CameraStreamWrapper* worker = new CameraStreamWrapper();
    worker->cameraStream = cameraStream;

    for(const auto&thread : m_threadsStorage)
    {
        if(!thread->isRunning())
        {
            m_threads.insert( worker->cameraStream->m_camera->getCameraId(),thread);
            worker->moveToThread(thread);

            bool isSignalConnected = connect(thread,SIGNAL(started()),cameraStream,SLOT(startStreamProcessingSlot()));
            isSignalConnected = connect(thread, &QThread::quit, worker, &QObject::deleteLater);
            isSignalConnected = connect(worker->cameraStream, SIGNAL(errorSignal(QUuid,QString)), this, SLOT(receiveStreamErrorSlot(QUuid,QString)));
            isSignalConnected = connect(worker->cameraStream,SIGNAL(finishedStreamProcessingSignal(QUuid)), this, SLOT(receiveFinishedStreamSlot(QUuid)));
            isSignalConnected = connect(worker->cameraStream,SIGNAL(sendCurrentFrameSignal(QImage*)), this, SLOT(receiveFrameSlot(QImage*)));

            // de adaugat metoda de delete latter
            // save video thread
            isSignalConnected = connect(worker->cameraStream, &cv::CameraStream::violationDetectedSignal, m_saveVideoThread, &SaveVideoThread::saveViolationSlot);

            isSignalConnected = connect(this, SIGNAL(displayStreamInterestItems(QUuid,bool)),worker->cameraStream,SLOT(displayStreamInterestElements(QUuid,bool)));
            isSignalConnected = connect(this,SIGNAL(displayStreamSignal(QUuid)),worker->cameraStream,SLOT(displayStreamSlot(QUuid)));
            isSignalConnected = connect(this, SIGNAL(stopStreamProcessingSignal(bool)), worker->cameraStream,SLOT(stopStreamProcessingSlot(bool)));

            --m_freeThreads;
            break;
        }
    }
}

QtCharts::QBarSet *MainWindow::groupByHour()
{
    int a[6]={0};
    for(const auto&violation:m_violationsList)
    {
        switch (violation.getTime().time().hour())
        {
        case 5 ... 7:
            ++a[0];
            break;
        case 8 ... 10:
            ++a[1];
            break;
        case 11 ... 13:
            ++a[2];
            break;
        case 14 ... 16:
            ++a[3];
            break;
        case 17 ... 19:
            ++a[4];
            break;
        case 20 ... 24:
        case 0 ... 4:
            ++a[5];
            break;
        }

    }
    QtCharts::QBarSet* set = new QtCharts::QBarSet("Time slot.");
    for(int i = 0; i<6; ++i)
    {
        qDebug("GroupByHour: hour: %d \t value:%d",i,a[i]);
        *set<<a[i];
    }
    return set;
}

void MainWindow::writeXML(const Camera &camera, QString fileName)
{

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlStream(&file);
    xmlStream.setAutoFormatting(true);
    xmlStream.writeStartDocument();
    xmlStream.writeStartElement("camera");
    xmlStream.writeTextElement("name",camera.getName().remove(' '));
    xmlStream.writeTextElement("location",camera.getLocation().remove(' '));
    xmlStream.writeTextElement("violationsNumber",QString(m_violationsList.size() + '0'));
    xmlStream.writeStartElement("violations");

    for(const auto&violation :m_violationsList)
    {
        xmlStream.writeStartElement("violation");
        xmlStream.writeTextElement("dateTime", violation.getTime().toString());
        xmlStream.writeTextElement("type", cv::enumToString((RULE_TYPE)violation.getRuleType()).c_str());
        xmlStream.writeEndElement();
    }
    xmlStream.writeEndElement();
    xmlStream.writeEndElement();

    xmlStream.writeEndDocument();

    file.close();

}


const Camera &MainWindow::getCurrentCamera()
{
    for(const auto&camera : m_cameraList)
    {
        if(camera.getLocation().remove(' ') == ui->cbCameraLocation->currentText().remove(' ')
                && camera.getName().remove(' ') == ui->cbName->currentText().remove(' '))
        {
            return  camera;
        }
    }
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
        if(ui->cbCameraLocation->findText(camera.getLocation().remove(' '))==-1)
        {
            ui->cbCameraLocation->addItem(camera.getLocation().remove(' '));
            ui->cbCameraLocation->setCurrentText(camera.getLocation().remove(' '));
        }

        if(ui->cbCameraLocation->currentText() == camera.getLocation().remove(' '))
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
