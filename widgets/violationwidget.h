#ifndef VIOLATIONITEM_H
#define VIOLATIONITEM_H

#include "ocv/utils.h"
#include "Licenta_all_include.gen.h"
#include "QtConstants.h"


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
    ViolationWidget(const QDateTime&dateTime, RULE_TYPE violationType, const QString&photoLocation);
    ViolationWidget(const Violation&violation);

    QLabel *m_lblDate;
    QLabel *m_lblViolationType;
    QLabel *m_lblViolationPhoto;

};

#endif // VIOLATIONITEM_H
