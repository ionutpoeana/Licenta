#include "violationwidget.h"

ViolationWidget::ViolationWidget(const QDateTime&dateTime, RULE_TYPE violationType, const  QString&photoLocation)
{

    m_lblDate = new QLabel;
    m_lblDate->setText(dateTime.toString());

    m_lblViolationType = new QLabel;
    m_lblViolationType->setText(QString::fromUtf8(enumToString(violationType).c_str()));

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(m_lblDate);
    hLayout->addWidget(m_lblViolationType);

    QVBoxLayout*vLayout = new QVBoxLayout;

    m_lblViolationPhoto = new QLabel;

    Mat frame = imread(photoLocation.toStdString());
    cvtColor(frame,frame,COLOR_BGR2RGB);
    QImage image(frame.data,frame.cols, frame.rows, QImage::Format::Format_RGB888);
    qDebug()<<this->metaObject()->className()<<"\t Image location: "<<photoLocation<<endl;
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
}
