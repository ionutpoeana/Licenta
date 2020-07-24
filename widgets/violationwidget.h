#ifndef VIOLATIONITEM_H
#define VIOLATIONITEM_H

#include "ocv/utils.h"
#include "Licenta_all_include.gen.h"
#include "widgets/videoplayerwidget.h"


#include <QWidgetItem>
#include <QListWidgetItem>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

#include <QDateTime>


class ViolationWidget : public QWidget
{

public:
    ViolationWidget(const QDateTime&dateTime, RULE_TYPE violationType,  QString photoLocation);
    ViolationWidget(const Violation&violation);

    QString m_videoLocation;
    QLabel *m_lblDate;
    QLabel *m_lblViolationType;
    QLabel *m_lblViolationPhoto;
    static VideoPlayerWidget* m_videoPlayer;

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

};

#endif // VIOLATIONITEM_H
