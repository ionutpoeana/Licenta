#include "addcamerawidget.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

// creaza un camera stream
// loacatie
// oras
// videoLocation
// un vector de rules

AddCameraWidget::AddCameraWidget(QWidget *parent) : QWidget(parent)
{
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

    m_cameraStream = nullptr;
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
    if(m_cameraStream != nullptr)
        delete m_cameraStream;

    m_cameraStream =  new CameraStream;

    QString videoLocation = QFileDialog::getOpenFileName(this,"Open video",QDir::currentPath(),QStringLiteral("Video (*.mp4)"));
    if(videoLocation.size()!=0)
    {
        m_cameraStream->m_videoLocation = videoLocation.toStdString();
    }

}

void AddCameraWidget::m_pBtnSave_clicked()
{
    if(m_cameraStream->m_videoLocation.empty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please choose a video!");
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
    m_cameraStream->m_cameraLocation = m_leCameraLocation->text().toStdString();

    if(  m_leCameraName->text().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please provide a camera name!");
        msgBox.exec();
        return;
    }
    m_cameraStream->m_cameraName = m_leCameraName->text().toStdString();

    if(m_cameraStream->m_trafficRules.empty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please provide at least a traffic rule!");
        msgBox.exec();
        return;
    }

     emit cameraSaved(m_cameraStream);
     qDebug()<<"qDebug(): The camera has been set succesfully!";
}

void AddCameraWidget::m_pBtnSetUpRule_clicked()
{
    if(m_cameraStream->m_videoLocation.empty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please choose a video!");
        msgBox.exec();
    }

    RULE_TYPE ruleSelection =(RULE_TYPE) m_cbRuleType->currentIndex();
    Rule* rule = nullptr;
    switch (ruleSelection)
    {
    case RULE_TYPE::SEMAPHORE:
        rule = new Semaphore();
        break;
    case RULE_TYPE::FORBIDDEN_FORWARD:
        break;
    case RULE_TYPE::FORBIDDEN_ON_THE_LEFT:
        break;
    case RULE_TYPE::FORBIDDEN_ON_THE_RIGHT:
        break;
    case RULE_TYPE::FORBIDDEN_FORWARD_OR_ON_THE_LEFT:
        break;
    case RULE_TYPE::FORBIDDEN_FORWARD_OR_ON_THE_RIGHT:
        break;
    }

    if(rule!=nullptr)
    {
        VideoCapture capture(m_cameraStream->m_videoLocation);
        rule->setup(capture);
        m_cameraStream->m_trafficRules.push_back(rule);
    }

    qDebug()<<"qDebug(): The rule has been set succesfully!";
}
