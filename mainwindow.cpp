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
    m_displayVideoTimer = new QTimer(this);

    m_database = QSqlDatabase::addDatabase("QODBC");

    m_database.setDatabaseName(connectionString);

    m_saveVideoThread = new SaveVideoThread(m_database,nullptr);

    m_hasCrimeBeenCommitted = false;
    violationNumber = 0;
    TIME = 0;

    m_violationsDetails = new CameraViolationDetailsWidget(ui->gbCameraViolations);

    connect(m_displayVideoTimer,SIGNAL(timeout()),this,SLOT(readFrame()));
    connect(m_saveVideoThread,&SaveVideoThread::videoSaved,this,&MainWindow::receiveViolation);
    connect(ui->actionAdd_camera, &QAction::triggered,this,&MainWindow::on_actionAddCamera);

}

MainWindow::~MainWindow()
{
    delete m_displayVideoTimer;
    delete ui;
}

void MainWindow::receiveCameraStream(CameraStream *cameraStream)
{
    if(m_addCameraWidget!=nullptr)
    {
        delete m_addCameraWidget;
        m_addCameraWidget = nullptr;
    }

    m_cameraStream = cameraStream;
    ui->cbCameraLocation->addItem(QString::fromUtf8(m_cameraStream->m_cameraLocation.c_str()));
    ui->cbName->addItem(QString::fromUtf8(m_cameraStream->m_cameraName.c_str()));

    if(m_displayVideoTimer->isActive())
    {
        m_displayVideoTimer->stop();
    }

    if(capture.isOpened())
    {
        capture.~VideoCapture();
    }

    capture.open(m_cameraStream->m_videoLocation);
    if(!capture.isOpened())
    {
        qDebug()<<"Error at opening file "<<QString::fromUtf8(m_cameraStream->m_videoLocation.c_str());
        exit(EXIT_FAILURE);
    }

    // for 3 frame diffencing
    for (int i = 0; i <= BUFFER_SIZE; ++i)
    {
        capture.read(frameMat);
    }

    previousFrameMat = frameMat.clone();


    m_displayVideoTimer->start(NORMAL_FRAME_TIME);
}

void MainWindow::readFrame()
{
    if(capture.isOpened())
    {
        cv::Mat frameMat;
        QImage* qImage;

        if(!capture.read(frameMat))
            return;

        for(const auto&m_rule : m_cameraStream->m_trafficRules)
        {

            m_rule->drawComponentsInfOnFrame(previousFrameMat);

            namedWindow("mask",WINDOW_NORMAL);
            imshow("mask",m_rule->getAreaMask());

            namedWindow("motionMat",WINDOW_NORMAL);
            imshow("motionMat",m_rule->getMotionMatrix());

            proofOfCrime.push(previousFrameMat.clone());

            m_rule->update(frameMat.clone());


            m_hasCrimeBeenCommitted = m_rule->checkRuleViolation();
            if(proofOfCrime.empty())
                m_hasCrimeBeenCommitted = false;


            if(m_hasCrimeBeenCommitted)
            {
                ViolationProof *violationProof = new ViolationProof(string("TrecerePeRosu") + to_string(violationNumber) + string(".avi"),1);

                while(!proofOfCrime.empty())
                {
                    violationProof->m_violationVideo.push(proofOfCrime.front());
                    proofOfCrime.pop();
                }

                m_saveVideoThread->saveVideo(violationProof);
                ++violationNumber;
            }
            else if(proofOfCrime.size()>30)
            {
                proofOfCrime.pop();
            }

            previousFrameMat = frameMat.clone();
            m_rule->drawInfOnFrame(frameMat);
            m_rule->drawComponentsInfOnFrame(frameMat);


            cv::Mat frameMatCopy;
            cv::resize(frameMat,frameMatCopy,cv::Size(640,360),0,0,cv::INTER_NEAREST);
            cv::cvtColor(frameMatCopy,frameMatCopy,cv::COLOR_BGR2RGB);

            qImage = new QImage(frameMatCopy.data,frameMatCopy.cols, frameMatCopy.rows, QImage::Format::Format_RGB888);
            ui->lblVideo->setPixmap(QPixmap::fromImage(*qImage));
        }
        TIME+=NORMAL_FRAME_TIME;
    }
}

// TODO: adaugat codul perfectionat din visual studio
// de scris partea a 2-a a licentei


void MainWindow::receiveViolation(const Violation&v)
{
    m_violationsDetails->addViolationItem(v);
}

void MainWindow::on_actionAddCamera()
{
    if(m_addCameraWidget!=nullptr)
    {
        delete m_addCameraWidget;
    }
    m_addCameraWidget = new AddCameraWidget();
    connect(m_addCameraWidget,&AddCameraWidget::cameraSaved, this, &MainWindow::receiveCameraStream);
    qDebug()<<"A mers sa adaug o actiune";
}
