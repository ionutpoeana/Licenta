#pragma once

#include "constants.h"

#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>

#include <vector>
#include <string>


extern int DISTANCE_THRESHOLD;
class Component
{
public:
    static int index;
    int componentIndex;
    int nrPoints;
    int nrNoDetectionFrames;
    int maxDistance;
    bool matchFound;
    bool hasPassedLine;
    cv::Point2f center;
    cv::KalmanFilter kf;


    std::vector<cv::Point> centerHistory;
    // trebuie sa memorez punctele prin care trece component
    Component()
    {
        // kf init
        kf = cv::KalmanFilter(STATE_NUM, MEASURE_NUM, 0);

        kf.transitionMatrix = (cv::Mat_<float>(STATE_NUM, STATE_NUM) <<
            1, 0, 1, 0,
            0, 1, 0, 1,
            0, 0, 1, 0,
            0, 0, 0, 1);

        setIdentity(kf.measurementMatrix);
        setIdentity(kf.processNoiseCov, cv::Scalar::all(1e-2));
        setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-1));
        setIdentity(kf.errorCovPost, cv::Scalar::all(1));

        randn(kf.statePost, cv::Scalar::all(0), cv::Scalar::all(DISTANCE_THRESHOLD));

        centerHistory = std::vector<cv::Point>();
        center = cv::Point2f(0, 0);


        maxDistance = 0;
        matchFound = false;
        hasPassedLine = false;
        nrPoints = 0;
        nrNoDetectionFrames = 0;
        componentIndex = 0;
    }

    ~Component()
    {
        center.~Point_();
        kf.~KalmanFilter();
    }

    bool operator==(const Component& c) const {
        return this->center == c.center;
    }
};

