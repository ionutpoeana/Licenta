#include "toggleswitch.h"

ToggleSwitch::ToggleSwitch(QWidget *parent) :
    QAbstractButton(parent),
    m_status(false),
    m_margin(3),
    m_bodyBrush(Qt::lightGray)
{
    this->setBrush(QColor(Qt::darkGreen));
}

ToggleSwitch::ToggleSwitch(const QBrush &brush, QWidget *parent) :
    QAbstractButton(parent),
    m_status(false),
    m_margin(3),
    m_bodyBrush(Qt::lightGray)
{
    this->setBrush(brush);
}

void ToggleSwitch::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    if(true == this->isEnabled())
    {
        painter.setBrush(this->m_status ? this->brush() : Qt::black);
        painter.setOpacity(this->m_status ? 0.5 : 0.38);
        painter.setRenderHint(QPainter::Antialiasing, true);

        painter.drawRoundedRect(QRect(this->m_margin, this->m_margin, this->width() - 2 * this->m_margin,
            this->height() - 2 * this->m_margin), 7.5, 7.5);

        painter.setBrush(this->m_bodyBrush);
        painter.setOpacity(1.0);

        if(true == this->m_status)
        {
            painter.drawEllipse(QRectF(this->width() - this->height(), (this->height() / 2) - (this->height() / 2),
                this->height(), this->height()));
        }
        else
        {
            painter.drawEllipse(QRectF(0, (this->height() / 2) - (this->height() / 2), this->height(), this->height()));
        }
    }
    else
    {
        painter.setBrush(Qt::black);
        painter.setOpacity(0.12);

        painter.drawRoundedRect(QRect(this->m_margin, this->m_margin, this->width() - 2 * this->m_margin,
            this->height() - 2 * this->m_margin), 7.5, 7.5);

        painter.setOpacity(1.0);
        painter.setBrush(QColor("#BDBDBD"));

        painter.drawEllipse(QRectF((this->width() / 2), (this->height() / 2) - (this->height() / 2),
            this->height(), this->height()));
    }
}

void ToggleSwitch::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() & Qt::LeftButton)
    {
        this->m_status = !this->m_status;
        emit ToggleSwitch::statusChanged(this->m_status);
    }

    QAbstractButton::mouseReleaseEvent(event);
}

QSize ToggleSwitch::sizeHint(void) const
{
    return QSize(this->width() + (2 * this->m_margin), this->height() + (2 * this->m_margin));
}
