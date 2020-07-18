#pragma once



#include "component.h"
#include "utils.h"
#include "rule.h"

class Semaphore : public Rule
{
private:
    // the crosswalk line
    std::vector<Point> _semaphoreLightDelimiter;

    // semaphore bouding rect
    Rect _semaphoreBoundingRect;

    // semaphore image
    Mat _semaphoreImage;

    // coordinates of semaphore lights stored in keypoints
    SemaphoreLightCoordinates _semaphoreLightsCoordinates;

    // last semaphore color, used in intermitent yello case
    SEMAPHORE_LIGHT _lastSemaphoreLight = SEMAPHORE_LIGHT::GREEN;
    SEMAPHORE_LIGHT _currentSemaphoreLight = SEMAPHORE_LIGHT::GREEN;

    // contur of area in which semaphore light is checked, coordinates in local image
    std::vector<Point> _interestAreaContour;

    // bouding rectangle of area  in which semaphore light is checked, coordinates in global image
    Rect _interestAreaBoundingRect;

    // motion matrix used for conected components
    std::vector<std::vector<long long>> _interestAreaMotionMatrix;

    // current frame components
    std::list<Component> _currentFrameComponents;

    // components that are beeing tracked
    std::list<Component> _trackedComponents;

    // area image
    Mat _interestAreaImage;

    // gray level copy of previous frame
    Mat _grayPrevInterestAreaImage;

    // the mask created using 3 frame diferencing
    Mat _interestAreaMask;

    // a buffer which holds binary images for 3 frame differencing method
    Mat _areaBuffer[BUFFER_SIZE];

    // index of current frame, used for _areaBuffer update
    long long _frameIndex;

    // checks if a horisontal line is crossed
    bool checkLineCrossing(Point &a, Point &b);

    Scalar* m_colors;
public:
    Semaphore();

    // contains logic to create a semaphore from a vector of BUFFER_SIZE+1 frames
    void setup(VideoCapture capture)override;

    // updates the _image mat constantly
    void update(Mat frame)override;

    // create and set binary area buffer
    // frames contains BUFFER_SIZE + 1 frames
    void setupAreaBuffer(const std::vector<Mat> &frames);

    Mat getAreaMask()override;

    void drawInfOnFrame(Mat& frame)override;

    SEMAPHORE_LIGHT getLight();

    void drawComponentsInfOnFrame(Mat& frame)override;


    bool checkRuleViolation()override;

    Mat getMotionMatrix() override;

    RULE_TYPE getRuleType() override;

    ~Semaphore() {};
};


