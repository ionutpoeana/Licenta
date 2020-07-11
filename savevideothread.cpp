#include "savevideothread.h"

#include <direct.h>

SaveVideoThread::SaveVideoThread(  QSqlDatabase _database, QObject *parent)
    :QThread(parent)
{
    m_abort = false;
    m_dataBase = _database;
}

SaveVideoThread::~SaveVideoThread()
{
    m_mutex.lock();
    m_abort = true;
    m_waitCondition.wakeOne();
    m_mutex.unlock();

    wait();
}

void SaveVideoThread::saveVideo(ViolationProof *violationProof)
{
    QMutexLocker locker(&m_mutex);

    m_videoQueue.push(violationProof);

    if(!isRunning())
    {
        start(NormalPriority);
    }
    else
    {
        m_waitCondition.wakeOne();
    }
}

void SaveVideoThread::run()
{
    forever{

        ViolationProof* violationProof = nullptr;
        m_mutex.lock();

        if(m_videoQueue.empty())
        {
            m_waitCondition.wait(&m_mutex);
        }
        else
        {
            violationProof = m_videoQueue.front();

            m_videoQueue.pop();
        }

        m_mutex.unlock();

        if(m_abort)
            return;

        if(violationProof !=nullptr)
        {
            int codec = cv::VideoWriter::fourcc('M','J','P', 'G');
            cv::VideoWriter videoWriter(violationProof->m_videoName,codec,30,violationProof->m_violationVideo.front().size(),true);

            cv::Mat proof;

            for (; !violationProof->m_violationVideo.empty();)
            {
                proof = violationProof->m_violationVideo.front().clone();
                violationProof->m_violationVideo.pop();
                videoWriter.write(proof);
            }

            videoWriter.release();


            char buffer[_MAX_FNAME] = {'\0'};
            if(!_getcwd(buffer,_MAX_FNAME))
            {
                qDebug("Eroare la preularea caii fisierului %s!\n",violationProof->m_videoName.c_str());
            }
            else
            {
                QString videoLocation(buffer);
                videoLocation.append("\\");
                videoLocation.append(QString::fromUtf8(violationProof->m_videoName.c_str()));

                QString photoLocation(videoLocation);
                photoLocation.replace(".avi",".png");

                QDateTime dateTime = QDateTime::currentDateTime();

                Violation violation;
                violation.setTime(dateTime);
                violation.setRuleType(violationProof->m_violationType);
                violation.setPhotoLocation(photoLocation);
                violation.setVideoLocation(videoLocation);

                qx::dao::insert(violation,&m_dataBase);

                m_dataBase.commit();

                emit videoSaved(violation);
            }
        }
    }
}
