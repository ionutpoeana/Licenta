#include "camerastream.h"
#include "utils.h"


#include <QDebug>

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
        qDebug()<<this->metaObject()->className()<<"\tStop stream processsing! Reached end of stream!";
        m_readFrameTimer->stop();
        return;
    }

    cv::resize(m_currentFrame,m_currentFrame,VIDEO_RESOLUTION);
    //qDebug()<<this->metaObject()->className() << "\t" <<m_currentFrame.rows << m_currentFrame.cols;
    int key = waitKey(1);
    if(key==32)
        m_readFrameTimer->stop();

    for(const auto &rule : m_trafficRules)
    {
        rule->drawInfOnFrame(m_prevFrame);
        rule->drawComponentsInfOnFrame(m_prevFrame);


        //namedWindow("mask",WINDOW_NORMAL);
        //imshow("mask",rule->getAreaMask());

        //namedWindow("motionMat",WINDOW_NORMAL);
        //imshow("motionMat",rule->getMotionMatrix());

        m_proofOfCrime.push(m_prevFrame.clone());

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
                violationProof->m_violationVideo.push(m_proofOfCrime.front());
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

        //qDebug()<<"CameraStream:\tm_violationProofSize="<<m_proofOfCrime.size();
    }

    if(m_isStreamDisplayed)
    {
        cvtColor(m_prevFrame,m_prevFrame, COLOR_BGR2RGB);

        Mat resized;
        cv::resize(m_prevFrame,resized,Size(1280,720));
        QImage* image = new QImage(resized.data,resized.cols, resized.rows, QImage::Format::Format_RGB888);
        emit sendCurrentFrameSignal(image);
    }
    m_prevFrame = m_currentFrame.clone();
    //qDebug()<<this->metaObject()->className() << "\t" <<m_prevFrame.rows << m_prevFrame.cols;
}

CameraStream::CameraStream(std::string saveLocation)
{
    m_saveLocation = saveLocation;
    m_readFrameTimer = new QTimer();
    connect(m_readFrameTimer,&QTimer::timeout,this, &CameraStream::readFrame);

}

CameraStream::~CameraStream()
{
    m_readFrameTimer->stop();
    if(m_readFrameTimer!=nullptr)
        delete m_readFrameTimer;
}

void CameraStream::startStreamProcessing()
{
    m_readFrameTimer->start(NORMAL_FRAME_TIME);
    m_videoCapture.open(m_camera->getStreamLocation().toStdString());

    m_videoCapture.read(m_prevFrame);
    cv::resize(m_prevFrame, m_prevFrame, VIDEO_RESOLUTION);

}

void CameraStream::displayStreamSlot(QUuid streamId)
{
    if(m_camera->getCameraId() == streamId)
        m_isStreamDisplayed = true;
    else
        m_isStreamDisplayed = false;
}
