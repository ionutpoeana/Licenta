#pragma once



#include "component.h"
#include "utils.h"
#include "rule.h"

class Semaphore : public Rule
{
private:
    // semaphore bouding rect
    Rect _boundingRect;

    // semaphore image
    Mat _image;

    // contur of area in which semaphore light is checked, coordinates in local image
    std::vector<Point> _areaContour;

    // bouding rectangle of area  in which semaphore light is checked, coordinates in global image
    Rect _areaContourBoundingRect;

    // motion matrix used for conected components
    std::vector<std::vector<long long>> _areaMotionMatrix;

    // current frame components
    std::list<Component> _currentFrameComponents;

    //
    std::list<Component> _components;

    // area image
    Mat _areaImage;

    Mat _cvtPrevAreaImage;

    // the mask created using 3 frame diferencing
    Mat _areaMask;

    // a buffer which holds binary images for 3 frame differencing method
    Mat _areaBuffer[BUFFER_SIZE];

    // the crosswalk line
    std::vector<Point> _delimiter;

    long long _frameIndex;

    bool checkLineCrossing(Point &a, Point &b);
public:
    Semaphore():_frameIndex(0){};

    // contains logic to create a semaphore from a vector of BUFFER_SIZE+1 frames
    void setup(VideoCapture capture);

    // updates the _image mat constantly
    void update(Mat frame);

    // create and set binary area buffer
    // frames contains BUFFER_SIZE + 1 frames
    void setupAreaBuffer(const std::vector<Mat> &frames);

    Mat getAreaMask();

    void drawInfOnFrame(Mat& frame);

    SEMAPHORE_LIGHT getLight();

    void drawComponentsInfOnFrame(Mat& frame);

    bool checkRuleViolation();

    Mat getMotionMatrix();

    virtual ~Semaphore() {};
};


