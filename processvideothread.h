#ifndef PROCESSVIDEOTHREAD_H
#define PROCESSVIDEOTHREAD_H

#include "ocv/utils.h"
#include "ocv/semaphore.h"

#include <QThread>


class ProcessVideoThread : public QThread
{
public:
    ProcessVideoThread();
    ~ProcessVideoThread();

    void saveVideo(std::string videoName, std::queue<cv::Mat> video);

signals:
    void savedVideo(const QString videoName);

    cv::VideoCapture m_capture;
    Semaphore m_semaphore;
    queue<Mat> m_proofOfCrime;
    cv::Mat m_frameMat;
    cv::Mat m_previousFrameMat;
    int m_violation;


protected:
    void run() override;

};

// trebuie sa citesc cate un cadru
// il procesez si il trimit la main
//

#endif // PROCESSVIDEOTHREAD_H
