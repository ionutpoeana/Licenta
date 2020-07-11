#include "cameraviolationdetailswidget.h"



CameraViolationDetailsWidget::CameraViolationDetailsWidget(QWidget *parent) : QWidget(parent)
{

    m_violations = new QList<ViolationWidget*>;

    m_vViolationsLayout = new QVBoxLayout;

    m_scrollAreaWidget = new QWidget;

    m_lblViolationsNumber = new QLabel;

    fLayout = new QFormLayout;
    vLayout_widget = new QVBoxLayout;
    gbDetails = new QGroupBox;
    gbDetails->setParent(parent);

    m_scrollAreaWidget->setObjectName("m_scrollAreaWidget");
    m_scrollAreaWidget->setLayout(m_vViolationsLayout);

    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidget(m_scrollAreaWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_lblViolationsNumber->setObjectName("m_lblViolationsNumber");

    fLayout->addRow("Violations number:",m_lblViolationsNumber);

    vLayout_widget->addLayout(fLayout);
    vLayout_widget->addWidget(m_scrollArea);

    gbDetails->setLayout(vLayout_widget);
    gbDetails->setFixedSize(340,300);
    gbDetails->show();
}

CameraViolationDetailsWidget::~CameraViolationDetailsWidget()
{

    delete m_vViolationsLayout;
    delete m_scrollArea;
    delete m_scrollAreaWidget;
    delete m_lblViolationsNumber;

}

void CameraViolationDetailsWidget::addViolationItems(const QList<Violation> violations)
{

    for(const auto&violation:violations)
    {
        ViolationWidget* vWidget = new ViolationWidget(violation);
        m_violations->push_back(vWidget);
        m_vViolationsLayout->addWidget(vWidget);
    }
    m_lblViolationsNumber->setText(QString::number(m_violations->size()));

}

void CameraViolationDetailsWidget::addViolationItem(const Violation violation)
{
    ViolationWidget* vWidget = new ViolationWidget(violation);
    m_violations->push_back(vWidget);
    m_vViolationsLayout->addWidget(vWidget);
    m_lblViolationsNumber->setText(QString::number(m_violations->size()));

}
