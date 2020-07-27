#include "violationchartwidget.h"

ViolationChartWidget::ViolationChartWidget( QtCharts::QBarSeries *series, QWidget *parent) : QWidget(parent)
{
    m_series = series;
    m_chart = new QtCharts::QChart();
    m_chart->addSeries(series);
    m_chart->setTitle("Camera violations");
    m_chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    m_chart->setMargins(QMargins(0,4,4,0));

    QStringList categories;
    categories << "5-8" << "8-11" << "11-14" << "14-17" << "17-20" << "20-23";

    m_axisX = new QtCharts::QBarCategoryAxis();
    m_axisX->append(categories);
    m_axisX->setTitleText("Time");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    series->attachAxis(m_axisX);

    m_axisY = new QtCharts::QValueAxis();
    m_axisY->setRange(0,10);
    m_axisY->setTickCount(5);
    m_axisY->setLabelFormat("%d");
    m_axisY->setTitleText("Violations no.");

    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    series->attachAxis(m_axisY);

    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    m_chartView = new QtCharts::QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setParent(this);
    m_chartView->setFixedSize(320,230);
    this->setFixedSize(320,230);
    this->show();
}

void ViolationChartWidget::removeChartSeries(QtCharts::QBarSeries *series)
{
    m_chart->removeAllSeries();

    m_series = series;

    m_chart->addSeries(m_series);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);
}
