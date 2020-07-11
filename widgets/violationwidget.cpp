#include "violationwidget.h"

ViolationWidget::ViolationWidget(const QDateTime&dateTime, int violationType,const  QString&photoLocation)
{

    m_lblDate = new QLabel;
    m_lblDate->setText(dateTime.toString());

    m_lblViolationType = new QLabel;
    m_lblViolationType->setText("QString::number(violationType)");

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(m_lblDate);
    hLayout->addWidget(m_lblViolationType);

    QVBoxLayout*vLayout = new QVBoxLayout;

    m_lblViolationPhoto = new QLabel;
    QImage image(photoLocation);

    m_lblViolationPhoto->setPixmap(QPixmap::fromImage(image.scaled(280,220)));

    m_lblViolationPhoto->setFrameShape(QFrame::Panel);
    m_lblViolationPhoto->setLineWidth(1);


    vLayout->addLayout(hLayout);
    vLayout->addWidget(m_lblViolationPhoto);

    QGroupBox*gBox = new QGroupBox;
    gBox->setParent(this);
    gBox->setLayout(vLayout);
    gBox->setFixedSize(290,240);

    this->setFixedSize(290,240);
}


ViolationWidget::ViolationWidget(const Violation &violation) :ViolationWidget(violation.getTime(),violation.getRuleType(),violation.getPhotoLocation())
{
}
