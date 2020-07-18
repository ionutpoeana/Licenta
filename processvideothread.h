#ifndef PROCESSVIDEOTHREAD_H
#define PROCESSVIDEOTHREAD_H

#include "ocv/utils.h"
#include "ocv/semaphore.h"
#include "ocv/camerastream.h"

#include <QThread>
#include <QWaitCondition>

class ProcessVideoThread : public QThread
{
private:
    CameraStream* m_cameraStream = nullptr;
    QMutex m_mutex;
    QWaitCondition m_pauseCondition;
    bool m_pause;
public:
    ProcessVideoThread();
    ~ProcessVideoThread();

    void pause();
    void resume();

protected:
    void run() override;

};

// trebuie sa citesc cate un cadru
// il procesez si il trimit la main
//

#endif // PROCESSVIDEOTHREAD_H
