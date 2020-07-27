#pragma once

#include <opencv2/core.hpp>
#include <QString>

#define SPACE_KEY 32
#define ESC 27
#define REPEAT 114

#define WAIT_TIME 10
#define NORMAL_FRAME_TIME 200

#define SEMAPHORE_THRESHOLD  45
#define BLACK_TOP_HAT_KERNEL_SIZE 5
#define COLOR_THRESHOLD 60

#define BOX_SIZE 10
#define MIN_BOX_NUM BER 5
#define MOTION_SPLIT_FACTOR 2

#define BUFFER_SIZE 3

#define MAX_FRAMES_NO_MOVEMENT_TIME 2
#define MAX_COMPONENT_UNDETECTED_NR_FRAMES 6
#define MAX_COMPONENT_NOT_VISIBLE_NR_FRAMES 3
#define MIN_DETECTED_COMPONENT_NR_FRAMES 10

#define STATE_NUM 4
#define MEASURE_NUM 2
#define KALMAN_TRAINING_POINTS_NUMBER 8
#define MAXIMUM_KALMAN_PREDICTION_TIMES 10



#define DIRECTION_SLOPE_POINTS_NUMBER 20



#define VIOLATION_PROOF_FRAMES 32
#define MIN_VIOLATION_PROOF_FRAMES 24

#define VIDEO_RESOLUTION cv::Size(1280,720)
#define FRAME_RATE 24


const float ATAN_15 = 0.267;
const float ATAN_165 = -0.267;

const float ATAN_30 = 0.577;
const float ATAN_150 = -0.577;

const float ATAN_45 = 1.5;
const float ATAN_135 = -1.5;


enum class SEMAPHORE_LIGHT {
    RED= 0,
    GREEN,
    YELLOW,
    UNDEFINED
};


enum  RULE_TYPE
{
    SEMAPHORE=0,
    FORBIDDEN_FORWARD,
    FORBIDDEN_ON_THE_LEFT,
    FORBIDDEN_FORWARD_OR_ON_THE_LEFT,
    FORBIDDEN_ON_THE_RIGHT,
    FORBIDDEN_FORWARD_OR_ON_THE_RIGHT
};



struct SemaphoreLightCoordinates
{
    bool hasAllLightsCenters = false;
    // highest one, smallest y
    cv::KeyPoint red;
    //
    cv::KeyPoint yellow;
    //
    cv::KeyPoint green;
};

const cv::Scalar GREEN = cv::Scalar(0, 255, 0);
const cv::Scalar BLUE = cv::Scalar(255, 0, 0);
const cv::Scalar RED = cv::Scalar(0, 0, 255);



