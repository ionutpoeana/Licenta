#ifndef CAMERASTREAM_H
#define CAMERASTREAM_H


#include "rule.h"
#include "Licenta_all_include.gen.h"
#include "violationproof.h"

#include <opencv2/videoio.hpp>

#include <QTimer>
#include <QWidget>

#include <queue>
class CameraStream :public QObject
{
    Q_OBJECT
    std::string m_saveLocation;
    bool m_hasCrimeBeenCommitted = false;
    bool m_addFramesAtCrimeProof = false;
    std::queue<cv::Mat> m_proofOfCrime;
    cv::VideoCapture m_videoCapture;
    cv::Mat m_currentFrame;
    cv::Mat m_prevFrame;
    unsigned int m_violationNumber;
    QTimer* m_readFrameTimer;
    bool m_isStreamDisplayed;

    void readFrame();
public:
    std::vector<Rule*> m_trafficRules;
    QSharedPointer<Camera> m_camera;

    CameraStream(std::string saveLocation);
    ~CameraStream();
    void startStreamProcessing();

public slots:
    void displayStreamSlot(QUuid streamId);

signals:
    void errorSignal(QString error);
    void finishedStreamProcessingSignal();

    void sendCurrentFrameSignal( QImage* image);
    void violationDetectedSignal(ViolationProof * violationProof);

};

#endif // CAMERASTREAM_H
