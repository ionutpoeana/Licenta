#include "violationwidget.h"
VideoPlayerWidget* ViolationWidget::m_videoPlayer;

ViolationWidget::ViolationWidget(const QDateTime&dateTime, RULE_TYPE violationType,   QString photoLocation)
{

    m_lblDate = new QLabel;
    m_lblDate->setText(dateTime.toString());

    m_lblViolationType = new QLabel;
    m_lblViolationType->setText(QString::fromUtf8(cv::enumToString(violationType).c_str()));

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(m_lblDate);
    hLayout->addWidget(m_lblViolationType);

    QVBoxLayout*vLayout = new QVBoxLayout;

    m_lblViolationPhoto = new QLabel;

    cv::Mat frame = cv::imread(photoLocation.toStdString());
    qDebug()<<this->metaObject()->className()<<"\t Image location: "<<photoLocation<<endl;
    cvtColor(frame,frame,cv::COLOR_BGR2RGB);
    QImage image(frame.data,frame.cols, frame.rows, QImage::Format::Format_RGB888);
    m_lblViolationPhoto->setPixmap(QPixmap::fromImage(image.scaled(270,220)));

    m_lblViolationPhoto->setFrameShape(QFrame::Panel);
    m_lblViolationPhoto->setLineWidth(1);


    vLayout->addLayout(hLayout);
    vLayout->addWidget(m_lblViolationPhoto);

    QGroupBox*gBox = new QGroupBox;
    gBox->setParent(this);
    gBox->setLayout(vLayout);
    gBox->setFixedSize(280,240);

    this->setFixedSize(280,240);
}


ViolationWidget::ViolationWidget(const Violation &violation) :ViolationWidget(violation.getTime(),(RULE_TYPE)violation.getRuleType(),violation.getPhotoLocation())
{
    m_videoLocation = violation.getVideoLocation();
}

void ViolationWidget::mouseDoubleClickEvent(QMouseEvent *event)
{

    if(m_videoPlayer !=nullptr)
        delete m_videoPlayer;

    m_videoPlayer = new VideoPlayerWidget();
    m_videoPlayer->setUrl(QUrl::fromLocalFile(m_videoLocation.remove(' ')));
    m_videoPlayer->play();
    m_videoPlayer->setFixedSize(1300,800);
    m_videoPlayer->show();

    qDebug()<<this->metaObject()->className()<<"\tMouseDoubleClickEvent: "<<m_videoLocation;
}
