#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include "violationchartwidget.h"

#include <QObject>
#include <QWidget>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QComboBox>

// un widget
// un stacked layout
// 2 radio buttons
// 2 obiecte de tip widget

class ChartWidget : public QWidget
{
    Q_OBJECT

    ViolationChartWidget* m_cameraChart = nullptr;
    ViolationChartWidget* m_allCameraChart = nullptr;
    QComboBox* m_cbChart = nullptr;
    QStackedWidget* m_stackedWidget = nullptr;

public:
    ChartWidget();
};

#endif // CHARTWIDGET_H
