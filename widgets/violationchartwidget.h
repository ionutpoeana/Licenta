#ifndef VIOLATIONCHARTWIDGET_H
#define VIOLATIONCHARTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>

class ViolationChartWidget : public QWidget
{
    Q_OBJECT
    QtCharts::QBarSeries *m_series;
    QtCharts::QChartView *m_chartView;
    QtCharts::QBarSet *m_set;
    QtCharts::QChart *m_chart;
    QStringList m_categories;
    QtCharts::QBarCategoryAxis *m_axisX ;
    QtCharts::QValueAxis *m_axisY;
    QtCharts::QChartView *chartView;

public:

    explicit ViolationChartWidget( QtCharts::QBarSeries *series, QWidget *parent = nullptr);
    ~ViolationChartWidget();

    void removeChartSeries(QtCharts::QBarSeries *series);

signals:

};



#endif // VIOLATIONCHARTWIDGET_H
