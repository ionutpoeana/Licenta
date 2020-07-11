#ifndef PROOF_H
#define PROOF_H

#include "constants.h"

#include <queue>

class ViolationProof
{
public:
    std::string m_videoName;
    int m_violationType;
    std::queue<cv::Mat> m_violationVideo;

    ViolationProof(std::string videoName, int violationType);

};

#endif // PROOF_H
