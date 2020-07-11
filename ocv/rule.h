#ifndef RULE_H
#define RULE_H

#include "constants.h"

#include <opencv2/videoio.hpp>

class Rule
{
public:
    Rule();

    RULE_TYPE m_ruleType;

    virtual ~Rule();

    virtual void setup( cv::VideoCapture capture) = 0;
    virtual void update(cv::Mat frame) = 0;
    virtual bool checkRuleViolation() = 0;

    virtual void drawInfOnFrame(cv::Mat &frame)=0;
    virtual void drawComponentsInfOnFrame(cv::Mat &frame) = 0;

    virtual cv::Mat getAreaMask(){return cv::Mat();};
    virtual cv::Mat getMotionMatrix(){return cv::Mat();};

};

#endif // RULE_H
