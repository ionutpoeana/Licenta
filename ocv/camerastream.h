#ifndef CAMERASTREAM_H
#define CAMERASTREAM_H


#include "rule.h"

class CameraStream
{

public:
    std::string m_videoLocation;
    std::string m_cameraLocation;
    std::string m_cameraName;
    RULE_TYPE m_ruleType;
    std::vector<Rule*> m_trafficRules;
    CameraStream();
};

#endif // CAMERASTREAM_H
