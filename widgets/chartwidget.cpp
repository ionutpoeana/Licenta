#include "chartwidget.h"

ChartWidget::ChartWidget()
{
    m_stackedWidget = new QStackedWidget;
    m_cameraChart = new ViolationChartWidget(new QtCharts::QBarSeries);
    m_allCameraChart = new ViolationChartWidget(new QtCharts::QBarSeries);
    m_stackedWidget->addWidget(m_cameraChart);
    m_stackedWidget->addWidget(m_allCameraChart);
    m_cbChart = new QComboBox;
    m_cbChart->addItem("Current camera");
    m_cbChart->addItem("All cameras");

    connect(m_cbChart, QOverload<int>::of(&QComboBox::activated), m_stackedWidget, &QStackedWidget::setCurrentIndex);
}
