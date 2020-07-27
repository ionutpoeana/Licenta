#include "processstreamthread.h"

#include <QtDebug>

int ProcessStreamThread::m_threadNumber = 0;

ProcessStreamThread::ProcessStreamThread()
{
    m_threadId = m_threadNumber;
    ++m_threadNumber;
}



