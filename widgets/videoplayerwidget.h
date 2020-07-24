#ifndef VIDEOPLAYERWIDGET_H
#define VIDEOPLAYERWIDGET_H

#include <QMediaPlayer>
#include <QWidget>

class QAbstractButton;
class QSlider;
class QLabel;
class QUrl;

class VideoPlayerWidget : public QWidget
{
    Q_OBJECT
public:
    VideoPlayerWidget(QWidget *parent = nullptr);
    ~VideoPlayerWidget();

    void setUrl(const QUrl &url);

public slots:
    void play();

private slots:
    void mediaStateChanged(QMediaPlayer::State state);
    void positionChanged(int position);
    void durationChanged(int duration);
    void setPosition(int position);
    void handleError();

private:
    QMediaPlayer* m_mediaPlayer;
    QAbstractButton *m_playButton;
    QSlider *m_positionSlider;
    QLabel *m_errorLabel;
};

#endif
