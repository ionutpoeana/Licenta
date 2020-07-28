#include "violationchartwidget.h"

ViolationChartWidget::ViolationChartWidget( QtCharts::QBarSeries *series, QStringList categories, QString xTitle, QString yTitle, QWidget *parent) : QWidget(parent)
{
    m_series = series;
    m_chart = new QtCharts::QChart();
    m_chart->addSeries(m_series);
    m_chart->setTitle("Camera violations");
    m_chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    m_chart->setMargins(QMargins(0,4,4,0));

    m_axisX = new QtCharts::QBarCategoryAxis();
    m_axisX->append(categories);
    //m_axisX->setTitleText(xTitle);
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_series->attachAxis(m_axisX);

    m_axisY = new QtCharts::QValueAxis();
    m_axisY->setRange(0,6);
    m_axisY->setLabelFormat("%d");
    m_axisY->setTitleText(yTitle);

    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisY);

    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    m_chartView = new QtCharts::QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setParent(this);
    m_chartView->setFixedSize(320,230);
    m_chartView->setContentsMargins(5,5,5,5);
    this->setFixedSize(320,230);
    this->show();
}

ViolationChartWidget::~ViolationChartWidget()
{

    // ala trebuie sa doar sa le afiseze si sa faca toggle intre ele
}

void ViolationChartWidget::removeChartSeries(QtCharts::QBarSeries *series)
{
    m_chart->removeAllSeries();

    m_series = series;

    m_chart->addSeries(m_series);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);
}

void ViolationChartWidget::removeChartSeries(QtCharts::QBarSeries *series, QStringList categories)
{
    m_chart->removeAllSeries();
    m_series = series;

    m_chart->addSeries(m_series);
    if(m_axisX)
        delete m_axisX;

    m_axisX = new QtCharts::QBarCategoryAxis();
    m_axisX->append(categories);
    //m_axisX->setTitleText(xTitle);
    m_chart->addAxis(m_axisX, Qt::AlignBottom);

    m_series->attachAxis(m_axisY);
}

