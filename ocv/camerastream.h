#ifndef CAMERASTREAM_H
#define CAMERASTREAM_H


#include "rule.h"
#include "Licenta_all_include.gen.h"
#include "violationproof.h"

#include <opencv2/videoio.hpp>

#include <QTimer>
#include <QWidget>

#include <queue>

namespace cv {


class CameraStream :public QObject
{
    Q_OBJECT
    std::string m_saveLocation;
    bool m_hasCrimeBeenCommitted = false;
    bool m_addFramesAtCrimeProof = false;
    bool m_displayInterestElemets = false;
    bool m_isStreamDisplayed;

    std::queue<cv::Mat> m_proofOfCrime;
    cv::VideoCapture m_videoCapture;
    cv::Mat m_currentFrame;
    cv::Mat m_prevFrame;
    unsigned int m_violationNumber;
    unsigned int m_frameTime;
    QTimer* m_readFrameTimer = nullptr;


    void readFrame();
public:
    std::vector<Rule*> m_trafficRules;
    QSharedPointer<Camera> m_camera;

    CameraStream(std::string saveLocation);
    virtual ~CameraStream();


public slots:
    void stopStreamProcessingSlot(bool stopOrStartStreamProcessing);
    void displayStreamSlot(QUuid streamId);
    void displayStreamInterestElements(QUuid streamId, bool areDisplayed);
    void startStreamProcessingSlot();

signals:
    void errorSignal(QUuid streamId, QString error);
    void finishedStreamProcessingSignal(QUuid streamId);

    void sendCurrentFrameSignal( QImage* image);
    void violationDetectedSignal(ViolationProof * violationProof);

};
}
#endif // CAMERASTREAM_H
