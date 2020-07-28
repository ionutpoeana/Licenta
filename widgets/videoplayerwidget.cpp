#include "videoplayerwidget.h"

#include <QtWidgets>
#include <QVideoWidget>

VideoPlayerWidget::VideoPlayerWidget(QWidget *parent)
    : QWidget(parent)
{
    m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    QVideoWidget *videoWidget = new QVideoWidget;


    m_playButton = new QPushButton;
    m_playButton->setEnabled(false);
    m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(m_playButton, &QAbstractButton::clicked, this, &VideoPlayerWidget::play);

    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setRange(0, 0);

    connect(m_positionSlider, &QAbstractSlider::sliderMoved,
            this, &VideoPlayerWidget::setPosition);

    m_errorLabel = new QLabel;
    m_errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(m_playButton);
    controlLayout->addWidget(m_positionSlider);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget);
    layout->addLayout(controlLayout);
    layout->addWidget(m_errorLabel);

    setLayout(layout);

    m_mediaPlayer->setVideoOutput(videoWidget);
    connect(m_mediaPlayer, &QMediaPlayer::stateChanged,
            this, &VideoPlayerWidget::mediaStateChanged);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayerWidget::positionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoPlayerWidget::durationChanged);
    connect(m_mediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, &VideoPlayerWidget::handleError);
}

VideoPlayerWidget::~VideoPlayerWidget()
{
}

void VideoPlayerWidget::setUrl(const QUrl &url)
{
    m_errorLabel->setText(QString());
    setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());
    qDebug()<<"URL:\t"<<url;
    m_mediaPlayer->setMedia(url);

    m_playButton->setEnabled(true);
}

void VideoPlayerWidget::play()
{
    switch (m_mediaPlayer->state()) {
    case QMediaPlayer::PlayingState:
        m_mediaPlayer->pause();
        break;
    default:
        m_mediaPlayer->play();
        break;
    }
}

void VideoPlayerWidget::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void VideoPlayerWidget::positionChanged(int position)
{
    m_positionSlider->setValue(position);
}

void VideoPlayerWidget::durationChanged(int duration)
{
    m_positionSlider->setRange(0, duration);
}

void VideoPlayerWidget::setPosition(int position)
{
    m_mediaPlayer->setPosition(position);
}

void VideoPlayerWidget::handleError()
{
    m_playButton->setEnabled(false);
    const QString errorString = m_mediaPlayer->errorString();
    QString message = "Error: ";
    if (errorString.isEmpty())
        message += " #" + QString::number(int(m_mediaPlayer->error()));
    else
        message += errorString;
    m_errorLabel->setText(message);
}
