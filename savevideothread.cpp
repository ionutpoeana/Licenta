#include "savevideothread.h"
#include "ocv/constants.h"

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

void SaveVideoThread::saveViolationSlot(ViolationProof *violationProof)
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
            std::string location;
            location.append(violationProof->m_saveLocation);
            location.append("\\");
            location.append(violationProof->m_camera->getLocation().toStdString());
            location.append(violationProof->m_camera->getName().toStdString());
            location.append(std::to_string(violationProof->m_violationNumber));

            std::string videoLocation = location;
            videoLocation.append(".avi");
            std::string photoLocation = location.append(".png");
            cv::VideoWriter videoWriter(videoLocation,codec,FRAME_RATE,violationProof->m_violationVideo.front().size(),true);

            cv::Mat proof;

            for (; !violationProof->m_violationVideo.empty();)
            {
                proof = violationProof->m_violationVideo.front().clone();
                violationProof->m_violationVideo.pop();
                if( violationProof->m_violationVideo.size()==MIN_VIOLATION_PROOF_FRAMES)
                {
                    cv::imwrite(photoLocation,proof);
                }
                videoWriter.write(proof);
            }

            videoWriter.release();


            QDateTime dateTime = QDateTime::currentDateTime();

            Violation* violation = new Violation;
            violation->setViolationId(QUuid::createUuid());
            violation->setTime(dateTime);
            violation->setRuleType((int)violationProof->m_violationType);
            violation->setPhotoLocation(QString::fromStdString(photoLocation));
            violation->setVideoLocation(QString::fromStdString(videoLocation));
            violation->setCamera(violationProof->m_camera);

            m_mutex.lock();
            QSqlError error = qx::dao::insert(violation,&m_dataBase);
            m_mutex.unlock();

            if(error.type()!=QSqlError::NoError)
            {
                qDebug()<<this->metaObject()->className() << ":\tQSqlError at violation insertion!";
            }
            else
            {
                qDebug()<<this->metaObject()->className() << ":\tViolation succesfully saved!"<< "Camera: "<<violationProof->m_camera->getName();

                m_dataBase.commit();
                delete violationProof;
                violationProof = nullptr;
                emit violationSavedSignal(violation);
            }

        }
    }
}
