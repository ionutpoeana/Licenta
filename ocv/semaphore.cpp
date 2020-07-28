#include "semaphore.h"

#include <QtDebug>

extern int DISTANCE_THRESHOLD;
namespace cv {



bool Semaphore::checkLineCrossing(Point &a, Point &b)
{

    // a, b and component coordinates are in same matrix ( _interestAreaBoudingRect)
    double delimiterSlope = 0;

    if ((b.x - a.x) != 0)
    {
        delimiterSlope = double(b.y - a.y) / double(b.x - a.x) ;
    }

    for ( auto& comp : _trackedComponents)
    {
        if(comp.hasPassedLine)
            continue;

        int y =  delimiterSlope*(comp.center.x - a.x) + a.y;
        if ((comp.center.y <=y )  && !comp.centerHistory.empty())
        {
            // component has passed _semaphoreLightDelimiter line
            int yHistory =   (comp.centerHistory.back().x - a.x)*delimiterSlope + a.y;
            if (comp.centerHistory.back().y > yHistory || comp.centerHistory.rbegin()[1].y > yHistory)
            {

                comp.hasPassedLine = true;
                // go in centerHistory vector to see if the component has come along
                // at least 1/3 of interest area height
                int imageHeight = (_interestAreaBoundingRect.height - a.y)/1.5 ;
                for(vector<Point>::reverse_iterator it = comp.centerHistory.rbegin(); it!=comp.centerHistory.rend(); ++it)
                {
                    if(it->y - a.y > imageHeight)
                    {

                        // check component direction
                        float meanX = 0;
                        float meanY = 0;
                        int centerHistorySize = comp.centerHistory.size();
                        int nOfPoints =  centerHistorySize>DIRECTION_SLOPE_POINTS_NUMBER ? DIRECTION_SLOPE_POINTS_NUMBER : centerHistorySize;
                        for(int i =0; i<nOfPoints ; ++i)
                        {
                            meanX+=comp.centerHistory[i].x;
                            meanY+=comp.centerHistory[i].y;
                        }
                        meanX /= nOfPoints;
                        meanY /= nOfPoints;

                        float sumDenominator = 0;
                        float sumNominator = 0;
                        for(int i =0; i<nOfPoints; ++i)
                        {
                            sumNominator+=((comp.centerHistory[i].x-meanX)*(comp.centerHistory[i].y - meanY));
                            sumDenominator+=pow((comp.centerHistory[i].x - meanX),2);
                        }

                        // we can't compute atan of lines
                        if(sumNominator == 0 || sumDenominator ==0)
                            continue;

                        float componentSlope = sumNominator/sumDenominator;
                        qDebug("Semaphore: componenentSlope =%2.3f, nominator = %2.3f, deonminator = %2.3f", componentSlope,sumNominator, sumDenominator);
                        // _interestAreaContour has 4 points in order {bottomLeft, topLeft, topRight, topLeft}
                        float interestAreaLineSlope = float(_interestAreaContour[0].y - _interestAreaContour[3].y)/ float(_interestAreaContour[0].x - _interestAreaContour[3].x);

                        float atan = (interestAreaLineSlope - componentSlope)/(1+interestAreaLineSlope*componentSlope);

                        qDebug("Semaphore: interestAreaLineSlope = %2.3f Points:\t A{%d %d} B{%d %d}",interestAreaLineSlope,_interestAreaContour[0].x,_interestAreaContour[0].y,_interestAreaContour[1].x,_interestAreaContour[1].y);
                        qDebug("Semaphore: componentIndex =%d \t atan=%2.3f\tinterestAreaLineSlope =%2.3f \tcomponentSlope =%2.3f\n\n",comp.componentIndex, atan,interestAreaLineSlope,componentSlope);

                        if(atan < ATAN_45 && atan >ATAN_135)
                        {
                            // there has been a crime
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

Semaphore::Semaphore():_frameIndex(0){
    m_colors =new Scalar[20];
    for( unsigned int i =1; i<20; ++i)
    {
        m_colors[i] = Scalar(rand()%255,rand()%255,rand()%255);
    }
}

bool Semaphore::isRuleSet()
{
    return m_isRuleSet;
}

void Semaphore::setup( VideoCapture &capture)
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
        resize(frameMat,frameMat,VIDEO_RESOLUTION);
        frames.push_back(frameMat.clone());
    }

    frameMat = frames[BUFFER_SIZE].clone();
    _frameIndex = 0;

    _semaphoreBoundingRect = selectROI("Select semaphore area!",frameMat);

    _interestAreaContour = getObjectContour("Select area of which Semaphore light does apply", frameMat, _interestAreaBoundingRect);

    destroyAllWindows();
    DISTANCE_THRESHOLD = _interestAreaBoundingRect.height/4;
    vector<vector<Point>> contours;
    contours.push_back(_interestAreaContour);

    for (auto& point : contours[0])
    {
        point.x += _interestAreaBoundingRect.x;
        point.y += _interestAreaBoundingRect.y;
    }

    drawContours(frameMat, contours, 0, RED, 2);
    _semaphoreLightDelimiter = getSemaphoreDelimiterArea("Area of which vehicles are not allowed to pass during red light", Mat(frameMat,_interestAreaBoundingRect));

    _interestAreaMotionMatrix = alocateMotionMatrix(_interestAreaBoundingRect);


    for (auto& point : contours[0])
    {
        point.x -= _interestAreaBoundingRect.x;
        point.y -= _interestAreaBoundingRect.y;
    }

    _interestAreaImage = cropObject(Mat(frames[BUFFER_SIZE - 1], _interestAreaBoundingRect), contours);
    cvtColor(_interestAreaImage, _grayPrevInterestAreaImage,COLOR_BGR2GRAY);

    destroyAllWindows();

    this->setupAreaBuffer(frames);
    this->update(frames[BUFFER_SIZE]);

    m_isRuleSet = true;
}

void Semaphore::update(Mat frame)
{
    _semaphoreImage = Mat(frame, _semaphoreBoundingRect);
    vector<vector<Point>> contours;
    contours.push_back(_interestAreaContour);
    _interestAreaImage = cropObject(Mat(frame, _interestAreaBoundingRect), contours);

    Mat grayCurrentAreaImage;
    Mat absDiff;

    cvtColor(_interestAreaImage, grayCurrentAreaImage, COLOR_BGR2GRAY);
    absdiff(_grayPrevInterestAreaImage, grayCurrentAreaImage, absDiff);

    _grayPrevInterestAreaImage = grayCurrentAreaImage.clone();

    threshold(absDiff, absDiff, COLOR_THRESHOLD, 255, THRESH_BINARY);

    Mat areaMaskCopy = _interestAreaMask.clone();;
    bitwise_xor(_interestAreaMask, _areaBuffer[_frameIndex % BUFFER_SIZE], _interestAreaMask);
    bitwise_and(_interestAreaMask, areaMaskCopy, _interestAreaMask);
    bitwise_or(_interestAreaMask, absDiff, _interestAreaMask);

    _areaBuffer[_frameIndex % BUFFER_SIZE] = absDiff.clone();

    processMotionMat(_interestAreaMask, _interestAreaMotionMatrix);

    _currentFrameComponents = getComponents(_interestAreaMotionMatrix);

    if (_trackedComponents.empty())
    {
        for (auto comp : _currentFrameComponents)
        {
            comp.componentIndex = Component::index;
            _trackedComponents.push_front(comp);
            ++Component::index;
        }
    }
    else
    {
        matchCurrentFrameComponenets(_currentFrameComponents, _trackedComponents);
    }

    //qDebug()<<"TIME:\t"<<TIME<<"\t_frameIndex:\t"<<_frameIndex;
    ++_frameIndex;
}

void Semaphore::setupAreaBuffer(const vector<Mat>& frames)
{
    Mat interestArea;
    Mat grayPrevInterestArea;

    vector<vector<Point>> contours;
    contours.push_back(_interestAreaContour);

    interestArea = cropObject(Mat(frames[0], _interestAreaBoundingRect), contours);
    cvtColor(interestArea, grayPrevInterestArea, COLOR_BGR2GRAY);
    _interestAreaMask = Mat(interestArea.rows, interestArea.cols, CV_8UC1, Scalar::all(0));

    Mat grayCurrentInterestArea;
    Mat currentInterestArea;
    Mat absDiff;
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        currentInterestArea = cropObject(Mat(frames[i + 1], _interestAreaBoundingRect), contours);
        cvtColor(currentInterestArea, grayCurrentInterestArea, COLOR_BGR2GRAY);

        absdiff(grayPrevInterestArea, grayCurrentInterestArea, absDiff);
        threshold(absDiff, absDiff, 50, 255, THRESH_BINARY);

        bitwise_or(_interestAreaMask, absDiff, _interestAreaMask);

        _areaBuffer[i] = absDiff.clone();

        grayPrevInterestArea = grayCurrentInterestArea.clone();
    }

    _frameIndex = BUFFER_SIZE;
}

Mat Semaphore::getAreaMask()
{

    return _interestAreaMask;
}

void Semaphore::drawInfOnFrame(Mat& frame)
{
    rectangle(frame, _semaphoreBoundingRect, RED);
    Point offset(_interestAreaBoundingRect.x, _interestAreaBoundingRect.y);
    line(frame, _semaphoreLightDelimiter[0] +offset , _semaphoreLightDelimiter[1] + offset, RED,4);
    vector<Point> globalContour;
    switch (_currentSemaphoreLight)
    {
    case SEMAPHORE_LIGHT::RED:
        putText(frame, "RED", Point(100, 100), FONT_HERSHEY_PLAIN, 5, Scalar(0, 0, 255), 5);
        break;
    case SEMAPHORE_LIGHT::GREEN:
        putText(frame, "GREEN", Point(100, 100), FONT_HERSHEY_PLAIN, 5, Scalar(0, 0, 255), 5);
        break;
    case SEMAPHORE_LIGHT::YELLOW:
        putText(frame, "YELLOW", Point(100, 100), FONT_HERSHEY_PLAIN, 5, Scalar(0, 0, 255), 5);
        break;
    default:
        putText(frame, "UNDEFINED", Point(100, 100), FONT_HERSHEY_PLAIN, 5, Scalar(0, 0, 255), 5);
        break;
    }

    for (const auto& point : _interestAreaContour)
    {
        globalContour.push_back(Point(point.x + _interestAreaBoundingRect.x, point.y + _interestAreaBoundingRect.y));
    }
    vector<vector<Point>> contours;
    contours.push_back(globalContour);
    drawContours(frame, contours, 0, RED, 2);
}


void Semaphore::drawComponentsInfOnFrame(Mat& frame)
{
    for (const auto& comp : _trackedComponents)
    {
        if(comp.isVisible)
        {
            if (comp.hasPassedLine)
            {
                putText(frame, to_string(comp.componentIndex), Point(comp.center.x + _interestAreaBoundingRect.x, comp.center.y + _interestAreaBoundingRect.y), FONT_HERSHEY_PLAIN, 2, RED, 2);
                circle(frame, Point(comp.center.x + _interestAreaBoundingRect.x, comp.center.y + _interestAreaBoundingRect.y),4,Scalar(0,255,0),FILLED);
            }
            else
            {
                circle(frame, Point(comp.center.x + _interestAreaBoundingRect.x, comp.center.y + _interestAreaBoundingRect.y),4,Scalar(0,255,0),FILLED);
                putText(frame, to_string(comp.componentIndex), Point(comp.center.x + _interestAreaBoundingRect.x, comp.center.y + _interestAreaBoundingRect.y), FONT_HERSHEY_PLAIN, 2, GREEN, 2);
            }

            for (const auto& point : comp.centerHistory)
            {
                circle(frame, Point(point.x + _interestAreaBoundingRect.x, point.y + _interestAreaBoundingRect.y), 2, m_colors[comp.componentIndex%20], FILLED);
            }
        }
        /*
        else
        {

            circle(frame, Point(comp.center.x + _interestAreaBoundingRect.x, comp.center.y + _interestAreaBoundingRect.y),4,Scalar(0,255,0),FILLED);
            putText(frame, to_string(comp.componentIndex), Point(comp.center.x + _interestAreaBoundingRect.x, comp.center.y + _interestAreaBoundingRect.y), FONT_HERSHEY_PLAIN, 2, RED, 2);

            for (const auto& point : comp.centerHistory)
            {
                circle(frame, Point(point.x + _interestAreaBoundingRect.x, point.y + _interestAreaBoundingRect.y), 2, GREEN, FILLED);
            }
        }
*/
    }
}

bool Semaphore::checkRuleViolation()
{

    _lastSemaphoreLight = _currentSemaphoreLight;
    _currentSemaphoreLight = getLight();

    if(_currentSemaphoreLight==SEMAPHORE_LIGHT::RED)
        return checkLineCrossing(_semaphoreLightDelimiter[0], _semaphoreLightDelimiter[1]);

    return false;
}

Mat Semaphore::getMotionMatrix()
{
    return getMotionMat(_interestAreaMotionMatrix);
}

RULE_TYPE Semaphore::getRuleType()
{
    return RULE_TYPE::SEMAPHORE;
}

Semaphore::~Semaphore()
{
    if(m_colors !=nullptr)
    {
        delete m_colors;
        m_colors = nullptr;
    }
}

SEMAPHORE_LIGHT Semaphore::getLight()
{



    std::vector<cv::KeyPoint> lightKeypoints = getKeypoints(_semaphoreImage);

    for (const auto& circle : lightKeypoints)
    {
        if (circle.pt.x > _semaphoreImage.cols * 0.33 && circle.pt.x < _semaphoreImage.cols * 0.66)
        {
            if (circle.pt.y < _semaphoreImage.rows * 0.33)
            {
                return  SEMAPHORE_LIGHT::RED;
            }

            if (circle.pt.y > _semaphoreImage.rows * 0.66)
            {
                return  SEMAPHORE_LIGHT::GREEN;
            }

            if ( (circle.pt.y > _semaphoreImage.rows * 0.33 && circle.pt.y < _semaphoreImage.rows * 0.66 ) || _semaphoreLightsCoordinates.yellow.size > 0 )
            {
                return  SEMAPHORE_LIGHT::YELLOW;
            }
        }
    }

    return  _lastSemaphoreLight;

    {
        /*

    int red = 0;
    int green = 0;
    int yellow = 0;

    if (_semaphoreLightsCoordinates.red.size > 0)
    {
        red = nrLightedPixels(_semaphoreImage,_semaphoreLightsCoordinates.red,SEMAPHORE_LIGHT::RED);
    }

    if (_semaphoreLightsCoordinates.green.size > 0)
    {
        green = nrLightedPixels(_semaphoreImage, _semaphoreLightsCoordinates.green, SEMAPHORE_LIGHT::GREEN);
    }

    if (_semaphoreLightsCoordinates.yellow.size > 0)
    {
        yellow = nrLightedPixels(_semaphoreImage, _semaphoreLightsCoordinates.yellow, SEMAPHORE_LIGHT::YELLOW);
    }

    // gets here when semaphore changes colors
    // there are a few frames in which all the semaphore colors
    // are turned off

    if(red > green && red > yellow)
        return SEMAPHORE_LIGHT::RED;

    if(green > yellow && green > red)
        return SEMAPHORE_LIGHT::GREEN;

    if(yellow > green && yellow>red)
        return SEMAPHORE_LIGHT::YELLOW;



    if (_lastSemaphoreLight == SEMAPHORE_LIGHT::RED)
        return SEMAPHORE_LIGHT::RED;

    return SEMAPHORE_LIGHT::UNDEFINED;
    */
    }
}
}
