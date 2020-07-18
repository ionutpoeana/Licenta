#include "addcamerawidget.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

// creaza un camera stream
// loacatie
// oras
// videoLocation
// un vector de rules

AddCameraWidget::AddCameraWidget( QSqlDatabase* database, QWidget *parent) : QWidget(parent)
{
    m_dataBase = database;

    m_pBtnOpenLocalVideo = new QPushButton;
    m_pBtnOpenLocalVideo->setObjectName("pBtnOpenLocalVideo");
    m_pBtnOpenLocalVideo->setText("Local video");

    m_pBtnOpenRTSPStream = new QPushButton;
    m_pBtnOpenRTSPStream->setObjectName("pBtnOpenRTSPStream");
    m_pBtnOpenRTSPStream->setText("RTSP stream");

    m_leCameraLocation= new QLineEdit;
    m_leCameraLocation->setObjectName("leCameraLocation");
    m_leCameraName= new QLineEdit;
    m_leCameraName->setObjectName("leCameraName");

    m_cbRuleType = new QComboBox;
    m_cbRuleType->setObjectName("m_cbRuleType");
    m_cbRuleType->addItem("RED LIGHT");
    m_cbRuleType->addItem("FORBIDDEN_FORWARD");
    m_cbRuleType->addItem("FORBIDDEN_FORWARD_OR_ON_THE_LEFT");
    m_cbRuleType->addItem("FORBIDDEN_ON_THE_RIGHT");
    m_cbRuleType->addItem("FORBIDDEN_FORWARD_OR_ON_THE_RIGHT");

    fLayout = new QFormLayout;
    fLayout->addRow("Camera location:",m_leCameraLocation);
    fLayout->addRow("Camera name:",m_leCameraName);
    fLayout->addRow("Ryle type:",m_cbRuleType);

    m_pBtnSetUpRule = new QPushButton;
    m_pBtnSetUpRule ->setObjectName("m_pBtnSetUpRule");
    m_pBtnSetUpRule ->setText("Setup rule");

    m_pBtnSave = new QPushButton;
    m_pBtnSave ->setObjectName("m_pBtnSave");
    m_pBtnSave ->setText("Save");


    gLayout = new QGridLayout;
    gLayout->addWidget(m_pBtnOpenLocalVideo,0,0,1,1);
    gLayout->addWidget(m_pBtnOpenRTSPStream,0,1,1,1);
    gLayout->addLayout(fLayout,1,0,3,2);
    gLayout->addWidget(m_pBtnSetUpRule,4,0,1,1);
    gLayout->addWidget(m_pBtnSave,4,1,1,1);


    gBox = new QGroupBox;
    gBox->setLayout(gLayout);
    gBox->setFixedSize(243,150);
    gBox->show();

    connect(m_pBtnOpenLocalVideo,&QPushButton::clicked,this,&AddCameraWidget::m_pBtnOpenLocalVideo_clicked);
    connect(m_pBtnSetUpRule,&QPushButton::clicked,this,&AddCameraWidget::m_pBtnSetUpRule_clicked);
    connect(m_pBtnSave,&QPushButton::clicked,this,&AddCameraWidget::m_pBtnSave_clicked);

    m_cameraStream = new CameraStream("F:\\violations");
    m_camera = new Camera;
}

AddCameraWidget::~AddCameraWidget()
{

    delete m_pBtnOpenLocalVideo;
    delete m_pBtnOpenRTSPStream;
    delete m_pBtnSave;
    delete m_pBtnSetUpRule;
    delete m_leCameraLocation;
    delete m_leCameraName;
    delete m_cbRuleType;

    delete fLayout;
    delete gLayout;
    delete gBox;
}

void AddCameraWidget::m_pBtnOpenLocalVideo_clicked()
{
    QString videoLocation = QFileDialog::getOpenFileName(this,"Open video",QDir::currentPath(),QStringLiteral("Video (*.mp4)"));
    if(videoLocation.size()!=0)
    {
        m_camera->setStreamLocation(videoLocation);
    }

}

void AddCameraWidget::m_pBtnSave_clicked()
{
    if(m_camera->getStreamLocation().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please choose a stream!");
        msgBox.exec();
        return;
    }

    if(m_leCameraLocation->text().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please provide a camera location!");
        msgBox.exec();
        return;
    }

    if(m_leCameraName->text().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please provide a camera name!");
        msgBox.exec();
        return;
    }

    qx_query cameraQuery;
    cameraQuery.where("t_camera.name").isEqualTo(m_leCameraName->text())
            .and_("t_camera.location").isEqualTo(m_leCameraLocation->text());

    m_mutex.lock();
    long cameraCount = qx::dao::count<Camera>(cameraQuery,m_dataBase);
    m_mutex.unlock();

    if(cameraCount>0)
    {
        QMessageBox msgBox;
        msgBox.setText("There already exists a camera with this name and location!");
        msgBox.exec();
        return;
    }


    if(m_cameraStream->m_trafficRules.empty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please provide at least a traffic rule!");
        msgBox.exec();
        return;
    }

    m_camera->setCameraId(QUuid::createUuid());
    m_camera->setLocation(m_leCameraLocation->text());
    m_camera->setName(m_leCameraName->text());

    m_mutex.lock();
    QSqlError error =  qx::dao::insert(m_camera,m_dataBase);
    m_mutex.unlock();
    if(error.type()!=QSqlError::NoError)
    {
        qDebug()<<this->metaObject()->className()<<"\tThe stream save has FAILED! "<<m_camera->getStreamLocation();

    }
    else
    {
        qDebug()<<this->metaObject()->className()<<"\tThe stream save has SUCCEDED! "<<m_camera->getStreamLocation();
        m_dataBase->commit();
        QSharedPointer<Camera> sharedCamera = QSharedPointer<Camera>(m_camera);
        m_cameraStream->m_camera = sharedCamera;
        emit cameraSavedSignal(m_cameraStream);
    }
}

void AddCameraWidget::m_pBtnSetUpRule_clicked()
{
    if(m_camera->getStreamLocation().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please choose a video!");
        msgBox.exec();
        return;
    }

    RULE_TYPE ruleSelection =(RULE_TYPE) m_cbRuleType->currentIndex();
    Rule* rule = nullptr;
    switch (ruleSelection)
    {
    case RULE_TYPE::SEMAPHORE:
        rule = new Semaphore();
        break;
    case RULE_TYPE::FORBIDDEN_FORWARD:
    case RULE_TYPE::FORBIDDEN_ON_THE_LEFT:
    case RULE_TYPE::FORBIDDEN_ON_THE_RIGHT:
    case RULE_TYPE::FORBIDDEN_FORWARD_OR_ON_THE_LEFT:
    case RULE_TYPE::FORBIDDEN_FORWARD_OR_ON_THE_RIGHT:
        QMessageBox msgBox;
        msgBox.setText("Rule not implemented!");
        msgBox.exec();
        break;
    }

    if(rule!=nullptr)
    {
        VideoCapture capture(m_camera->getStreamLocation().toStdString());
        rule->setup(capture);
        m_cameraStream->m_trafficRules.push_back(rule);
    }
    qDebug()<<this->metaObject()->className()<<"\tThe rule has been set succesfully!"<<QString::fromStdString(enumToString(rule->getRuleType()));
}
