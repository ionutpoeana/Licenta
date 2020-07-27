#include "camerastream.h"
#include "utils.h"


#include <QDebug>

namespace cv
{
void CameraStream::readFrame()
{

    if(!m_videoCapture.isOpened())
    {
        qDebug()<<this->metaObject()->className()<<"\tVideo capture error! Aborting execution!";
        m_readFrameTimer->stop();
        return;
    }


    if(!m_videoCapture.read(m_currentFrame))
    {
        qDebug()<<this->metaObject()->className()<<"\tSto   p stream processsing! Reached end of stream!";
        m_readFrameTimer->stop();
        emit finishedStreamProcessingSignal(m_camera->getCameraId());
        deleteLater();
        return;

    }

    cv::resize(m_currentFrame,m_currentFrame,VIDEO_RESOLUTION);
    //qDebug()<<this->metaObject()->className() << "\t" <<m_currentFrame.rows << m_currentFrame.cols;


    for(const auto &rule : m_trafficRules)
    {

        m_proofOfCrime.push(m_prevFrame.clone());

        if(m_displayInterestElemets)
        {
            rule->drawInfOnFrame(m_prevFrame);
            rule->drawComponentsInfOnFrame(m_prevFrame);
        }


        rule->update(m_currentFrame.clone());

        if(!m_hasCrimeBeenCommitted)
        {
            m_hasCrimeBeenCommitted = rule->checkRuleViolation();

            if(m_hasCrimeBeenCommitted)
            {
                m_addFramesAtCrimeProof = true;
            }

        }

        if(m_proofOfCrime.size()==VIOLATION_PROOF_FRAMES && m_addFramesAtCrimeProof)
        {
            m_addFramesAtCrimeProof = false;
            m_hasCrimeBeenCommitted = false;

            ViolationProof* violationProof = new ViolationProof;

            for(int i = 0;i<VIOLATION_PROOF_FRAMES/2; ++i)
            {
                violationProof->m_violationVideo.push(m_proofOfCrime.front().clone());
                m_proofOfCrime.pop();
            }

            for(int i = VIOLATION_PROOF_FRAMES/2;i<VIOLATION_PROOF_FRAMES; ++i)
            {
                violationProof->m_violationVideo.push(m_proofOfCrime.front());
                m_proofOfCrime.push(m_proofOfCrime.front().clone());
                m_proofOfCrime.pop();
            }

            violationProof->m_saveLocation = m_saveLocation;
            violationProof->m_violationType = rule->getRuleType();
            violationProof->m_camera = m_camera;
            violationProof->m_violationNumber = m_violationNumber;
            ++m_violationNumber;
            emit violationDetectedSignal(violationProof);
        }else if(!m_addFramesAtCrimeProof && m_proofOfCrime.size() > MIN_VIOLATION_PROOF_FRAMES)
        {
            m_proofOfCrime.pop();
        }
    }


    //qDebug("Thread for stream %s %s is running!", m_camera->getLocation().toStdString().c_str(),m_camera->getName().toStdString().c_str());


    if(m_isStreamDisplayed && m_videoCapture.isOpened())
    {
        //qDebug("Stream %s %s sent frame to MainThread", m_camera->getLocation().toStdString().c_str(),m_camera->getName().toStdString().c_str());
        cvtColor(m_prevFrame,m_prevFrame, COLOR_BGR2RGB);
        Mat resized;
        cv::resize(m_prevFrame,resized,VIDEO_RESOLUTION);
        QImage* image = new QImage(resized.data,resized.cols, resized.rows, QImage::Format::Format_RGB888);
        emit sendCurrentFrameSignal(image);
    }

    m_prevFrame = m_currentFrame.clone();
}

CameraStream::CameraStream(std::string saveLocation)
{
    m_saveLocation = saveLocation;
}

CameraStream::~CameraStream()
{

    if(m_readFrameTimer!=nullptr)
    {
        delete m_readFrameTimer;
        m_readFrameTimer = nullptr;
    }


    for(unsigned int i =0; i< m_trafficRules.size(); ++i)
    {

        delete m_trafficRules[i];
        m_trafficRules[i] = nullptr;
    }
}

void CameraStream::stopStreamProcessingSlot(bool stopOrStartStreamProcessing)
{
    if(stopOrStartStreamProcessing)
    {
        m_readFrameTimer->stop();
    }
    else
    {
        m_readFrameTimer->start(NORMAL_FRAME_TIME);
    }
}

void CameraStream::startStreamProcessingSlot()
{
    m_readFrameTimer = new QTimer();
    connect(m_readFrameTimer,&QTimer::timeout,this, &CameraStream::readFrame);

    m_readFrameTimer->start(NORMAL_FRAME_TIME);
    m_videoCapture.open(m_camera->getStreamLocation().toStdString());

    m_videoCapture.read(m_prevFrame);
    cv::resize(m_prevFrame, m_prevFrame, VIDEO_RESOLUTION);

}

void CameraStream::displayStreamSlot(QUuid streamId)
{
    if(m_camera->getCameraId() == streamId)
    {
        m_isStreamDisplayed = true;

        if(m_readFrameTimer!=nullptr && m_readFrameTimer->isActive())
        {
            qDebug()<<"Stream is displayed: Camera location: " << m_camera->getLocation() <<" name "<<m_camera->getName()<< "\tTimer is active";
        }
    }
    else
    {
        qDebug()<<"Stream is NOT displayed: Camera location: " << m_camera->getLocation() <<" name "<<m_camera->getName();
        m_isStreamDisplayed = false;
        m_displayInterestElemets = false;
    }
}

void CameraStream::displayStreamInterestElements(QUuid streamId, bool areDisplayed)
{
    if(m_camera->getCameraId() == streamId)
    {
        m_displayInterestElemets = areDisplayed;
        qDebug()<<"Interest elements are displayed: Camera location: " << m_camera->getLocation() <<" name "<<m_camera->getName();
    }
}
}
