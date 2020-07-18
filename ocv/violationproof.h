#ifndef PROOF_H
#define PROOF_H


#include "constants.h"
#include "Licenta_all_include.gen.h"

#include <QUuid>
#include <QSharedPointer>

#include <queue>

class ViolationProof
{
public:
    QSharedPointer<Camera> m_camera;
    std::string m_saveLocation;
    unsigned int m_violationNumber;
    RULE_TYPE m_violationType;
    std::queue<cv::Mat> m_violationVideo;

};

#endif // PROOF_H
