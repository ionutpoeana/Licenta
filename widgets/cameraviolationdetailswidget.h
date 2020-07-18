#ifndef CAMERAVIOLATIONDETAILS_H
#define CAMERAVIOLATIONDETAILS_H

#include "violationwidget.h"

#include <QWidget>
#include <QGroupBox>
#include <QLayout>
#include <QScrollArea>
#include <QLabel>
#include <QListWidget>
#include <QFormLayout>

class CameraViolationDetailsWidget : public QWidget
{
    Q_OBJECT

private:
    QFormLayout* fLayout;
    QVBoxLayout* vLayout_widget;
    QGroupBox* gbDetails;
public:
    explicit CameraViolationDetailsWidget(QWidget *parent = nullptr);
    ~CameraViolationDetailsWidget();


    void addViolationItems(const QList<Violation> violations);
    void addViolationItem(const Violation violation);

    QList<ViolationWidget*> *m_violations;

    QVBoxLayout* m_vViolationsLayout;
    QWidget* m_scrollAreaWidget;
    QLabel* m_lblViolationsNumber;
    QScrollArea* m_scrollArea;


signals:

};

#endif // CAMERAVIOLATIONDETAILS_H
