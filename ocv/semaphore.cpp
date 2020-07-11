#include "semaphore.h"

#include <QtDebug>

extern int DISTANCE_THRESHOLD;

bool Semaphore::checkLineCrossing(Point &a, Point &b)
{

    // consider horisontal orientation
    // a, b and component coordinates are in same matrix ( _areaBoudingRect)
    double m = double(b.y - a.y) / double(b.x - a.x);
    int y;
    int yLastHistory;
    for ( auto& comp : _components)
    {
        y = (comp.center.x - a.x)*m + a.y;

        if ((comp.center.y < y )  && !comp.centerHistory.empty())
        {
            // component has passed _delimiter line
            yLastHistory = (comp.centerHistory.back().x - a.x)*m + a.y;
            if (comp.centerHistory.back().y > yLastHistory)
            {
                comp.hasPassedLine = true;
                return true;
            }
            return false;
        }
    }
}

void Semaphore::setup( VideoCapture capture)
{

    if(!capture.isOpened())
    {
        qCritical("Eroare la deschiderea fisierului %s","ceva fisier");
        exit(EXIT_FAILURE);
    }

    Mat frameMat;
    vector<Mat> frames;
    for (int i = 0; i <= BUFFER_SIZE; ++i)
    {
        capture.read(frameMat);
        frames.push_back(frameMat.clone());
    }

    _frameIndex = 0;

    _boundingRect = selectROI(frames[BUFFER_SIZE]);

    _areaContour = getObjectContour("Area of which Semaphore light does apply", frames[BUFFER_SIZE], _areaContourBoundingRect);

    DISTANCE_THRESHOLD = _areaContourBoundingRect.height/4;
    vector<vector<Point>> contours;
    contours.push_back(_areaContour);
    for (auto& point : contours[0])
    {
        point.x += _areaContourBoundingRect.x;
        point.y += _areaContourBoundingRect.y;
    }
    drawContours(frames[BUFFER_SIZE], contours, 0, RED, 2);
    _delimiter = getSemaphoreDelimiterArea("Area of which vehicles are not allowed to pass during red light", frames[BUFFER_SIZE]);

    _areaMotionMatrix = alocateMotionMatrix(_areaContourBoundingRect);

    _areaImage = cropObject(Mat(frames[BUFFER_SIZE - 1], _areaContourBoundingRect), contours);

    cvtColor(_areaImage, _cvtPrevAreaImage,COLOR_BGR2GRAY);

    destroyAllWindows();

    this->setupAreaBuffer(frames);
    this->update(frames[BUFFER_SIZE]);

    capture.~VideoCapture();
}

void Semaphore::update(Mat frame)
{
    _image = Mat(frame, _boundingRect);
    vector<vector<Point>> contours;
    contours.push_back(_areaContour);

    // retin cvtul imaginii anteriore

    _areaImage = cropObject(Mat(frame, _areaContourBoundingRect), contours);

    Mat cvtAreaImage;
    Mat absDiff;
    cvtColor(_areaImage, cvtAreaImage, COLOR_BGR2GRAY);
    absdiff(_cvtPrevAreaImage, cvtAreaImage, absDiff);

    _cvtPrevAreaImage = cvtAreaImage.clone();

    threshold(absDiff, absDiff, 50, 255, THRESH_BINARY);

    Mat areaMaskCopy = _areaMask.clone();;
    bitwise_xor(_areaMask, _areaBuffer[_frameIndex % BUFFER_SIZE], _areaMask);
    bitwise_and(_areaMask, areaMaskCopy, _areaMask);

    _areaBuffer[_frameIndex % BUFFER_SIZE] = absDiff.clone();

    bitwise_or(_areaMask, absDiff, _areaMask);

    processMotionMat(_areaMask, _areaMotionMatrix);

    _currentFrameComponents = getComponents(_areaMotionMatrix);

    if (_components.empty())
    {
        for (auto comp : _currentFrameComponents)
        {
            comp.componentIndex = Component::index;
            _components.push_front(comp);
            ++Component::index;
        }
    }
    else
    {
        matchCurrentFrameComponenets(_currentFrameComponents, _components);
    }

    ++_frameIndex;
}

void Semaphore::setupAreaBuffer(const vector<Mat>& frames)
{
    Mat interestArea;
    Mat cvtInterestArea;

    vector<vector<Point>> contours;
    contours.push_back(_areaContour);

    interestArea = cropObject(Mat(frames[0], _areaContourBoundingRect), contours);
    cvtColor(interestArea, cvtInterestArea, COLOR_BGR2GRAY);
    _areaMask = Mat(interestArea.rows, interestArea.cols, CV_8UC1, Scalar::all(0));

    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        Mat currentInterestArea = cropObject(Mat(frames[i + 1], _areaContourBoundingRect), contours);
        Mat cvtCurrentInterestArea;
        cvtColor(currentInterestArea, cvtCurrentInterestArea, COLOR_BGR2GRAY);

        absdiff(cvtInterestArea, cvtCurrentInterestArea, cvtInterestArea);
        threshold(cvtInterestArea, cvtInterestArea, 50, 255, THRESH_BINARY);

        bitwise_or(_areaMask, cvtInterestArea, _areaMask);

        _areaBuffer[i] = cvtInterestArea.clone();

        cvtInterestArea = cvtCurrentInterestArea.clone();
    }

    _frameIndex = BUFFER_SIZE;
}

Mat Semaphore::getAreaMask()
{
    return _areaMask;
}

void Semaphore::drawInfOnFrame(Mat& frame)
{
    rectangle(frame, _boundingRect, GREEN);
    line(frame, _delimiter[0], _delimiter[1], 4);
    vector<Point> globalContour;
    for (const auto& point : _areaContour)
    {
        globalContour.push_back(Point(point.x + _areaContourBoundingRect.x, point.y + _areaContourBoundingRect.y));
    }
    vector<vector<Point>> contours;
    contours.push_back(globalContour);
    drawContours(frame, contours, 0, RED, 2);
}


void Semaphore::drawComponentsInfOnFrame(Mat& frame)
{
    for (const auto& comp : _components)
    {
        if (comp.hasPassedLine)
        {
            putText(frame, to_string(comp.componentIndex), Point(comp.center.x + _areaContourBoundingRect.x, comp.center.y + _areaContourBoundingRect.y), FONT_HERSHEY_PLAIN, 2, RED, 2);
            circle(frame, Point(comp.center.x + _areaContourBoundingRect.x, comp.center.y + _areaContourBoundingRect.y),4,Scalar(0,255,0),FILLED);
        }
        else
        {
            circle(frame, Point(comp.center.x + _areaContourBoundingRect.x, comp.center.y + _areaContourBoundingRect.y),4,Scalar(0,255,0),FILLED);
            putText(frame, to_string(comp.componentIndex), Point(comp.center.x + _areaContourBoundingRect.x, comp.center.y + _areaContourBoundingRect.y), FONT_HERSHEY_PLAIN, 2, GREEN, 2);

        }
        for (const auto& point : comp.centerHistory)
        {
            circle(frame, Point(point.x + _areaContourBoundingRect.x, point.y + _areaContourBoundingRect.y), 2, BLUE, FILLED);
        }
    }
}

bool Semaphore::checkRuleViolation()
{
    //SEMAPHORE_LIGHT SemaphoreLight = SEMAPHORE_LIGHT::GREEN;//= getLight();
    // verific daca culoarea semaforului si daca sunt obiecte care au trecut de linia respectiva
    /*if (_frameIndex % 300 > 0 && _frameIndex % 300 < 150)
        SemaphoreLight = SEMAPHORE_LIGHT::RED;


    switch (SemaphoreLight)
    {
    case SEMAPHORE_LIGHT::GREEN:
        return false;

    case SEMAPHORE_LIGHT::RED:*/

    if(getLight()==SEMAPHORE_LIGHT::RED)
        return checkLineCrossing(_delimiter[0], _delimiter[1]);
    //default:
    //	break;
    //}

        //return false;
}

Mat Semaphore::getMotionMatrix()
{
    return getMotionMat(_areaMotionMatrix);
}


SEMAPHORE_LIGHT Semaphore::getLight()
{
    // folosirea transformatei hough pentru detectarea cercurilor
    // verificarea intensitatilor pixelilor dintre cercuri
    // aplicarea unei logici pentru detectarea culorii

    Vec3b* ptr = _image.ptr<Vec3b>();
    int redPixels = 0;
    int greenPixels = 0;
    int yellowPixels = 0;

    for (int i = 0; i < _image.rows / 3; ++i)
    {
        ptr = _image.ptr<Vec3b>(i);
        for (int j = 0; j < _image.cols; ++j)
        {
            // bgr
            if (ptr[j][2] > SEMAPHORE_THRESHOLD)
            {
                ptr[j] = Vec3b(0, 0, 255);
                ++redPixels;
            }
            else
            {
                ptr[j] = Vec3b(0, 0, 0);
            }
        }
    }

    for (int i = _image.rows / 3; i < 2 * _image.rows / 3; ++i)
    {
        ptr = _image.ptr<Vec3b>(i);
        for (int j = 0; j < _image.cols; ++j)
        {
            if (ptr[j][1] > SEMAPHORE_THRESHOLD)
            {
                ptr[j] = Vec3b(0, 255, 255);
                ++yellowPixels;
            }
            else
            {
                ptr[j] = Vec3b(0, 0, 0);
            }
        }
    }

    for (int i = 2 * _image.rows / 3; i < _image.rows; ++i)
    {
        ptr = _image.ptr<Vec3b>(i);
        for (int j = 0; j < _image.cols; ++j)
        {

            if (ptr[j][1] > SEMAPHORE_THRESHOLD)
            {
                ptr[j] = Vec3b(0, 255, 0);
                ++greenPixels;
            }
            else
            {
                ptr[j] = Vec3b(0, 0, 0);
            }
        }
    }

    if (_frameIndex % 300 > 0 && _frameIndex % 300 < 150)
        return SEMAPHORE_LIGHT::RED;
    else
        return SEMAPHORE_LIGHT::GREEN;

    if (redPixels > greenPixels) return SEMAPHORE_LIGHT::RED;

    if (greenPixels > redPixels) return SEMAPHORE_LIGHT::GREEN;
}
