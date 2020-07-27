#ifndef PROCESSSTREAMTHREAD_H
#define PROCESSSTREAMTHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>

class ProcessStreamThread : public QThread
{
private:
    Q_OBJECT
    bool m_isRunning = false;
    static int m_threadNumber;
    int m_threadId;
public:
    ProcessStreamThread();
    int getThreadId(){return m_threadId;}

};

#endif // PROCESSSTREAMTHREAD_H
