#pragma once

#include <opencv2/core.hpp>
#include <QString>

#define SPACE_KEY 32
#define ESC 27
#define WAIT_TIME 10
#define NORMAL_FRAME_TIME 5
#define REPEAT 114

#define SEMAPHORE_THRESHOLD  150

#define BOX_SIZE 10
#define MIN_BOX_NUMBER 5

#define MAX_FRAMES_NO_MOVEMENT_TIME 2

#define BUFFER_SIZE 3
#define MOTION_SPLIT_FACTOR 4

#define MAXIMUM_UNDETECTED_NR_FRAMES 3

#define ZOOM_MAX_VALUE 10

#define STATE_NUM 4
#define MEASURE_NUM 2

enum class SEMAPHORE_LIGHT {
    RED,
    GREEN,
    YELLOW,
    UNDEFINED
};

enum class SEMAPHORE_TYPE {
    CARS,
    PEDESTRIANS
};

enum class OBJECT_ORIENTATION {
    VERTICAL,
    HORIZONTAL,
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


const cv::Scalar GREEN = cv::Scalar(0, 255, 0);
const cv::Scalar BLUE = cv::Scalar(255, 0, 0);
const cv::Scalar RED = cv::Scalar(0, 0, 255);

const QString connectionString =(
            "DRIVER={SQL Server Native Client 11.0};"
            "SERVER=(localdb)\\ProjectsV13;"
            "DATABASE=LICENTA;"
            "Trusted_Connection=Yes;"
       );

