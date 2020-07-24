#include "processstreamthread.h"

#include <QtDebug>

int ProcessStreamThread::m_threadNumber = 0;

ProcessStreamThread::ProcessStreamThread()
{
    m_threadId = m_threadNumber;
    ++m_threadNumber;
}

void ProcessStreamThread::pause()
{
    m_mutex.lock();
    qDebug()<<this->metaObject()->className() << "Thread paused: theradId:\t"<<m_threadId;
    m_isRunning = false;
    m_waitCondition.wait(&m_mutex);
    m_mutex.unlock();
}

void ProcessStreamThread::resume()
{
    m_mutex.lock();
    qDebug()<<this->metaObject()->className() << "Thread resumed: theradId:\t"<<m_threadId;
    m_isRunning = true;
    m_mutex.unlock();
    m_waitCondition.wakeOne();

}


