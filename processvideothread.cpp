#include "processvideothread.h"


ProcessVideoThread::ProcessVideoThread()
{

}

ProcessVideoThread::~ProcessVideoThread()
{

}

void ProcessVideoThread::pause()
{
    m_mutex.lock();
    m_pause = true;
    m_mutex.unlock();
}

void ProcessVideoThread::resume()
{
    m_mutex.lock();
    m_pause = false;
    m_mutex.unlock();

    m_pauseCondition.wakeOne();
}

void ProcessVideoThread::run()
{


}
