
#include "utils.h"

extern long long TIME;
extern int DISTANCE_THRESHOLD;

#include<QtDebug>

using namespace std;



namespace cv
{


bool isMotionInMat(Mat mat)
{
    if (countNonZero(mat) > ((mat.rows * mat.cols) >> MOTION_SPLIT_FACTOR))
    {
        return true;
    }
    return false;
}


void showFrame(std::string name, Mat& frame, WindowFlags flag)
{
    namedWindow(name, flag);
    resizeWindow(name,1280,720);
    moveWindow(name,100,100);
    imshow(name, frame);

}

void processMotionMat(Mat& mat, vector<vector<long long>>& motionMatrix)
{
    Mat box;
    int lines = motionMatrix.size();
    int cols = motionMatrix[0].size();


    // am un timp de inceput, 0
    // la fiecare procesare la care gasesc miscare, incrementez pozitia pe care am gasit miscare
    // cu valoarea curenta de asteptare a unui frame

    for (int i = 0; i < lines; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            Rect boxRect = Rect(j * BOX_SIZE, i * BOX_SIZE, BOX_SIZE, BOX_SIZE);
            box = Mat(mat, boxRect);
            if (isMotionInMat(box))
            {
                motionMatrix[i][j] = TIME;
            }
            else if (motionMatrix[i][j])
            {
                if ((motionMatrix[i][j] + NORMAL_FRAME_TIME * MAX_FRAMES_NO_MOVEMENT_TIME) < TIME)
                {
                    motionMatrix[i][j] = TIME - NORMAL_FRAME_TIME * MAX_FRAMES_NO_MOVEMENT_TIME;
                }
            }
        }
    }


}

Mat getMotionMat(vector<vector<long long>> motionMatrix)
{
    Mat motionMat = Mat::zeros(motionMatrix.size() * BOX_SIZE, motionMatrix[0].size() * BOX_SIZE, CV_8UC1);
    uchar* rowPtr;
    int rows = motionMatrix.size();
    int cols = motionMatrix[0].size();
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (motionMatrix[i][j] && motionMatrix[i][j] > (TIME - NORMAL_FRAME_TIME * MAX_FRAMES_NO_MOVEMENT_TIME))
            {
                int k = i * BOX_SIZE;
                int l = j * BOX_SIZE;
                for (int n = k; n < k + BOX_SIZE; ++n)
                {
                    rowPtr = motionMat.ptr<uchar>(n);
                    for (int m = l; m < l + BOX_SIZE; ++m)
                    {
                        rowPtr[m] = 255;
                    }
                }
            }
        }
    }
    return motionMat;

}


list<Component> getComponents(vector<vector<long long>>& motionMatrix)
{

    // padding the matrix on top and left side
    int rows = motionMatrix.size() + 1;
    int cols = motionMatrix[0].size() + 1;

    unsigned int** componentMatrix = (unsigned int**)malloc(rows * sizeof(unsigned int*));
    componentMatrix[0] = (unsigned int*)malloc(cols * sizeof(unsigned int));

    for (int i = 0; i < cols; ++i)
    {
        componentMatrix[0][i] = 0;
    }


    for (int i = 1; i < rows; ++i)
    {
        componentMatrix[i] = (unsigned int*)malloc(cols * sizeof(unsigned int));
        componentMatrix[i][0] = 0;
        for (int j = 1; j < cols; ++j)
        {
            if (motionMatrix[i-1][j-1] && motionMatrix[i - 1][j - 1] > (TIME - NORMAL_FRAME_TIME * MAX_FRAMES_NO_MOVEMENT_TIME))
            {
                componentMatrix[i][j] = 1;
            }
            else
            {
                componentMatrix[i][j] = 0;
            }
        }
    }

    unsigned int currentFrameComponentonent = 0;
    unsigned int conflict[500] = { 0 };


    --rows;
    --cols;
    for (int i = 1; i < rows; ++i)
    {
        for (int j = 1; j < cols; ++j)
        {
            // daca avem miscare

            if (componentMatrix[i][j] != 0)
            {

                // L 0 u 0
                if (componentMatrix[i][j - 1] == 0 && componentMatrix[i - 1][j] == 0)
                {
                    if (componentMatrix[i - 1][j - 1])
                    {
                        componentMatrix[i][j] = componentMatrix[i - 1][j - 1];
                    }
                    else if (componentMatrix[i - 1][j + 1])
                    {
                        componentMatrix[i][j] = componentMatrix[i - 1][j + 1];
                    }
                    else
                    {
                        ++currentFrameComponentonent;
                        componentMatrix[i][j] = currentFrameComponentonent;
                    }
                }
                // u == L
                else if (componentMatrix[i - 1][j] == componentMatrix[i][j - 1])
                {
                    componentMatrix[i][j] = componentMatrix[i - 1][j];
                }
                // u 1 L 0 - use 8 distance
                else if (componentMatrix[i][j - 1] == 0 && componentMatrix[i - 1][j])
                {
                    componentMatrix[i][j] = componentMatrix[i - 1][j];
                }
                // u 0 L 1
                else if (componentMatrix[i][j - 1] != 0 && componentMatrix[i - 1][j] == 0)
                {
                    componentMatrix[i][j] = componentMatrix[i][j - 1];
                }
                // u != L 1
                else if (componentMatrix[i][j - 1] != componentMatrix[i - 1][j])
                {
                    // L is a smaller component rank
                    if (componentMatrix[i][j - 1] < componentMatrix[i - 1][j])
                    {
                        componentMatrix[i][j] = componentMatrix[i][j - 1];
                        conflict[componentMatrix[i - 1][j]] = componentMatrix[i][j];
                    }
                    // U is a smaller component rank
                    else
                    {
                        componentMatrix[i][j] = componentMatrix[i - 1][j];
                        conflict[componentMatrix[i][j - 1]] = componentMatrix[i][j];
                    }

                }
            }

        }
    }

    Component trackedComponents[currentFrameComponentonent+2];
    Mat motionMat = Mat::zeros(motionMatrix.size() * (BOX_SIZE+1), motionMatrix[0].size() *  (BOX_SIZE+1), CV_8UC3);
    Vec3b* rowPtr;

    Scalar* colors =new Scalar[currentFrameComponentonent+2];
    for( unsigned int i =1; i<=currentFrameComponentonent; ++i)
    {
        colors[i] = Scalar(rand()%255,rand()%255,rand()%255);
    }

    for (int i = 0; i <= currentFrameComponentonent; ++i)
    {
        if(conflict[i]!=0)
        {
            int conf = conflict[i];
            while (conflict[conf])
            {
                conf = conflict[conf];
            }
            conflict[i] = conf;
        }
    }

    unsigned int element;
    for (int i = 1; i < rows; ++i)
    {
        for (int j = 1; j < cols; ++j)
        {
            element = componentMatrix[i][j];
            if (element != 0 && conflict[element] != 0)
            {

                componentMatrix[i][j] = conflict[componentMatrix[i][j]];
                // in our image the coordonate axis are changed
            }
            trackedComponents[componentMatrix[i][j]].center.x += j;
            trackedComponents[componentMatrix[i][j]].center.y += i;
            ++trackedComponents[componentMatrix[i][j]].nrPoints;
            int k = i * BOX_SIZE ;
            int l = j * BOX_SIZE ;
            for (int n = k; n < k + BOX_SIZE; ++n)
            {
                rowPtr = motionMat.ptr<Vec3b>(n);
                for (int m = l; m < l + BOX_SIZE; ++m)
                {
                    rowPtr[m][0] = colors[componentMatrix[i][j]][0];
                    rowPtr[m][1] = colors[componentMatrix[i][j]][1];
                    rowPtr[m][2] = colors[componentMatrix[i][j]][2];
                }
            }

        }
    }

    //showFrame("Motion mat in get components",motionMat,WINDOW_NORMAL);
    // take only the trackedComponents over avg number of fixels
    int avgComponentPixels = 0;
    if (currentFrameComponentonent > 0)
    {
        for (unsigned int i = 1; i <= currentFrameComponentonent; ++i)
        {
            avgComponentPixels += trackedComponents[i].nrPoints;
        }

        avgComponentPixels /= currentFrameComponentonent;
    }

    list<Component> filteredtrackedComponents;

    for (unsigned int i = 1; i <= currentFrameComponentonent; ++i)
    {
        if (trackedComponents[i].nrPoints > avgComponentPixels/MOTION_SPLIT_FACTOR)
        {
            trackedComponents[i].center.x /= trackedComponents[i].nrPoints;
            trackedComponents[i].center.y /= trackedComponents[i].nrPoints;

            Component component;
            component.center.x = trackedComponents[i].center.x * BOX_SIZE - BOX_SIZE / 2;
            component.center.y = trackedComponents[i].center.y * BOX_SIZE - BOX_SIZE / 2;
            component.nrPoints = trackedComponents[i].nrPoints;
            filteredtrackedComponents.push_back(component);
        }
    }

    for (int i = 0; i < rows; ++i)
    {
        free(componentMatrix[i]);
    }
    free(componentMatrix);
    delete[]colors;
    return filteredtrackedComponents;
}

vector<Point> orderContourPoints(vector<Point> contour)
{
    Point topLeft(10000000, 0), topRight(0, 0), bottomLeft(10000000, 0), bottomRight(0, 0);

    for (Point point : contour)
    {
        if (point.x < topLeft.x)
        {
            topLeft = point;
        }
    }

    for (Point point : contour)
    {
        if (point.x < bottomLeft.x && point != topLeft)
        {
            bottomLeft = point;
        }
    }

    if (topLeft.y < bottomLeft.y)
    {
        Point swap = topLeft;
        topLeft = bottomLeft;
        bottomLeft = swap;
    }

    for (Point point : contour)
    {
        if (point.x > topRight.x)
        {
            topRight = point;
        }
    }

    for (Point point : contour)
    {
        if (point.x > bottomRight.x && point != topRight)
        {
            bottomRight = point;
        }
    }

    if (topRight.y < bottomRight.y)
    {
        Point swap = topRight;
        topRight = bottomRight;
        bottomRight = swap;
    }

    return { topLeft, topRight, bottomRight, bottomLeft };
}

vector<vector<long long>> alocateMotionMatrix(Rect rectangle)
{
    vector<vector<long long>> motionMatrix(ceil(rectangle.height / BOX_SIZE));

    int width = ceil(rectangle.width / BOX_SIZE);
    for (auto& row : motionMatrix)
    {
        for (int i = 0; i < width; ++i)
        {
            row.push_back(0);
        }
    }
    return motionMatrix;
}


void matchCurrentFrameComponenets(list<Component>& currentFrametrackedComponents, list<Component>& trackedComponents)
{

    for (auto comp = trackedComponents.begin(); comp!=trackedComponents.end();)
    {
        // it component was created long ago and it doesn't have enought historyCenter points
        // we delete it
        if(TIME > comp->creationTime + MAX_COMPONENT_UNDETECTED_NR_FRAMES*NORMAL_FRAME_TIME)
        {
            if(comp->centerHistory.size() < (TIME - comp->creationTime)/(NORMAL_FRAME_TIME*2))
            {
                comp = trackedComponents.erase(comp);
            }
            else
            {
                comp->matchFound = false;
                ++comp;
            }
        }
        else
        {
            comp->matchFound = false;
            ++comp;
        }

    }


    double currentDistance;
    double minDistance;
    bool matchFound;
    Mat predictionMat;
    Mat measurementMat = Mat::zeros(MEASURE_NUM, 1, CV_32F);
    Point2f predictedPoint, nextCenterPoint;
    Point2f statePoint;
    for (auto& currentFrameComponent : currentFrametrackedComponents)
    {
        minDistance = DISTANCE_THRESHOLD;
        Component* trackedCompRef = nullptr;
        Component* currentFrameComponentMatchRef = nullptr;

        matchFound = false;

        for (auto& trackedComp : trackedComponents)
        {

            // calculate norm between predicted center and new found component
            // aria componentei curente sa fie cel putin jumatate cat aria componentei urmarite

            currentDistance = norm(currentFrameComponent.center - trackedComp.center);
            if ((currentDistance < minDistance) && !(trackedComp.matchFound || currentFrameComponent.matchFound))
            {
                predictionMat = trackedComp.kf.predict();
                predictedPoint = Point2f(predictionMat.at<float>(0, 0), predictionMat.at<float>(1, 0));

                // check number of kalman training points
                // after that we use only kalman filter prediction, corection and measurement
                if(trackedComp.centerHistory.size()<KALMAN_TRAINING_POINTS_NUMBER)
                {
                    nextCenterPoint = currentFrameComponent.center;
                }
                else
                {
                    //nextCenterPoint = norm(currentFrameComponent.center - trackedComp.center) < norm(trackedComp.center - predictedPoint) ? currentFrameComponent.center : predictedPoint;
                    nextCenterPoint = predictedPoint;
                }

                minDistance = currentDistance;
                trackedCompRef = &trackedComp;
                currentFrameComponentMatchRef = &currentFrameComponent;
                matchFound = true;
            }

        }

        if (matchFound)
        {
            // measurement is the point with smallest distance between currentComponenet and component
            measurementMat = Mat::zeros(MEASURE_NUM, 1, CV_32F);
            measurementMat.at<float>(0, 0) = currentFrameComponentMatchRef->center.x;
            measurementMat.at<float>(1, 0) = currentFrameComponentMatchRef->center.y;

            trackedCompRef->kf.correct(measurementMat);

            // update history vector
            trackedCompRef->centerHistory.push_back(trackedCompRef->center);

            // update the component center with predictedPoint
            trackedCompRef->center = nextCenterPoint;
            trackedCompRef->matchFound = true;
            currentFrameComponentMatchRef->matchFound = true;
            trackedCompRef->nrNoDetectionFrames = 0;
        }

    }

    for (auto currentFrameComponent : currentFrametrackedComponents)
    {
        if (!currentFrameComponent.matchFound)
        {
            currentFrameComponent.componentIndex = Component::index;
            ++Component::index;
            currentFrameComponent.isVisible = false;
            trackedComponents.push_front(currentFrameComponent);
        }
    }

    for (auto comp = trackedComponents.begin(); comp != trackedComponents.end();)
    {
        if(comp->centerHistory.size()>MIN_DETECTED_COMPONENT_NR_FRAMES)
        {
            comp->isVisible = true;

            if (!comp->matchFound)
            {
                // daca nu gasesc un match de 5 cadre
                // nu mai afisez componenta
                // daca nu gasesc un match de 8 cadre
                // sterg componenta
                comp->nrNoDetectionFrames++;
                if(comp->nrNoDetectionFrames>MAX_COMPONENT_NOT_VISIBLE_NR_FRAMES)
                {
                    comp->isVisible = false;
                }
            }
        }
        if (comp->nrNoDetectionFrames > MAX_COMPONENT_UNDETECTED_NR_FRAMES)
        {
            comp = trackedComponents.erase(comp);
        }
        else
        {
            ++comp;
        }

    }
}


void getDelimiterMouseCbk(int event, int x, int y, int, void* params)
{
    vector<Point>& contour = *(vector<Point>*)params;
    if (!event == EVENT_LBUTTONDOWN || contour.size() == 4)
    {
        return;
    }

    Point point(x, y);
    contour.push_back(point);
}

vector<Point> getSemaphoreDelimiterArea(String frameName, Mat frame)
{

    vector<Point> delimiterLine;
    void* params = (void*)&delimiterLine;
    vector<Point> delimiterLineSet;


    while (delimiterLine.size() != 4)
    {


        Mat frameClone = frame.clone();
        fprintf(stdout, "Chose 2 points for semaphore delimiter area!\n");
        showFrame(frameName, frameClone, WINDOW_NORMAL);
        setMouseCallback(frameName, getDelimiterMouseCbk, params);

        while (waitKey() != SPACE_KEY);

        if(delimiterLine[0].x > delimiterLine[1].x)
        {
            Point aux = delimiterLine[0];
            delimiterLine[0]=delimiterLine[1];
            delimiterLine[1] = aux;
        }

        polylines(frameClone, delimiterLine, true, Scalar(0, 0, 255), 4);
        showFrame(frameName, frameClone, WINDOW_NORMAL);

        int option = waitKey();

        if (option == SPACE_KEY)
        {
            int i = 0;
            for (Point point : delimiterLine)
            {
                if (i % 2 == 1)
                {
                    delimiterLineSet.push_back(point);
                }
                i++;
            }


            setMouseCallback(frameName, NULL, NULL);
            destroyWindow(frameName);
            return delimiterLineSet;

        }
        else if (option == ESC)
        {
            delimiterLine.clear();
            delimiterLineSet.clear();
            destroyWindow(frameName);
        }
    }
}


void getContourMouseCbk(int event, int x, int y, int, void* params)
{
    vector<Point>& contour = *(vector<Point>*)params;
    if (!event == EVENT_LBUTTONDOWN || contour.size() == 8)
    {
        return;
    }

    Point point(x, y);
    contour.push_back(point);
}

vector<Point> getObjectContour(string frameName, Mat frameMat, Rect& rect)
{

    vector<Point> contour;
    void* params = (void*)&contour;
    vector<Point> contourSet;

    while (contour.size() != 8)
    {

        Mat frameClone = frameMat.clone();
        showFrame(frameName, frameClone, WINDOW_NORMAL);
        setMouseCallback(frameName, getContourMouseCbk, params);


        while (waitKey() != SPACE_KEY);

        polylines(frameClone, contour, true, Scalar(0, 0, 255), 4);
        showFrame(frameName, frameClone, WINDOW_NORMAL);

        int option = waitKey();

        if (option == SPACE_KEY)
        {
            rect = boundingRect(contour);

            for (auto& point : contour)
            {
                point.x -= rect.x;
                point.y -= rect.y;
            }

            int i = 0;

            for (Point point : contour)
            {
                if (i % 2 == 1)
                {
                    contourSet.push_back(point);
                }
                i++;
            }
            setMouseCallback(frameName, NULL, NULL);
            destroyWindow(frameName);
            return orderContourPoints(contourSet);

        }
        else if (option == ESC)
        {
            contour.clear();
            contourSet.clear();
            destroyWindow(frameName);
        }
    }
}

Mat cropObject(Mat mat, vector<vector<Point>> contour)
{
    Mat mask(mat.rows, mat.cols, CV_8U, Scalar::all(0));

    drawContours(mask, contour, 0, Scalar::all(255), FILLED);

    Mat contourMat;
    mat.copyTo(contourMat, mask);

    return contourMat;
}


bool isCircleLightOn(Mat semaphoreMat, KeyPoint circle, SEMAPHORE_LIGHT light)
{
    int startRow = circle.pt.y - circle.size / 2;
    int stopRow = circle.pt.y + circle.size / 2;

    int startColl = circle.pt.x - circle.size / 2;
    int stopColl = circle.pt.x + circle.size / 2;

    Vec3b* ptr = nullptr;

    int nrGreenPixels = 0;
    int nrRedPixels = 0;

    switch (light)
    {
    case SEMAPHORE_LIGHT::RED:

        for (int i = startRow; i <= stopRow; ++i)
        {
            ptr = semaphoreMat.ptr<Vec3b>(i);
            for (int j = startColl; j < stopColl; ++j)
            {
                if (ptr[j][2] > COLOR_THRESHOLD)
                {
                    ++nrRedPixels;
                }
            }
        }
        if (nrRedPixels > (int(circle.size * circle.size) >> 1))
            return true;
        return false;
        break;
    case SEMAPHORE_LIGHT::GREEN:

        for (int i = startRow; i <= stopRow; ++i)
        {
            ptr = semaphoreMat.ptr<Vec3b>(i);
            for (int j = startColl; j < stopColl; ++j)
            {
                if (ptr[j][1] > COLOR_THRESHOLD)
                {
                    ++nrGreenPixels;
                }
            }
        }
        if (nrGreenPixels > (int(circle.size * circle.size) >> 1))
            return true;
        return false;
        break;
    case SEMAPHORE_LIGHT::YELLOW:
        for (int i = startRow; i <= stopRow; ++i)
        {
            ptr = semaphoreMat.ptr<Vec3b>(i);
            for (int j = startColl; j < stopColl; ++j)
            {
                if (ptr[j][1] > COLOR_THRESHOLD)
                {
                    ++nrGreenPixels;
                }

                if (ptr[j][2] > COLOR_THRESHOLD)
                {
                    ++nrRedPixels;
                }

            }
        }
        if (nrGreenPixels + nrGreenPixels > (int(circle.size * circle.size) >> 1))
            return true;
        return false;
        break;
    default:
        break;
    }
}

std::vector<KeyPoint> getKeypoints(Mat semaphoreMat)
{
    Mat semaphoreCopy;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(semaphoreMat.size() / BLACK_TOP_HAT_KERNEL_SIZE));

    cvtColor(semaphoreMat, semaphoreCopy, COLOR_BGR2GRAY);

    morphologyEx(semaphoreCopy, semaphoreCopy, MORPH_TOPHAT, kernel);

    threshold(semaphoreCopy, semaphoreCopy, SEMAPHORE_THRESHOLD, 255, THRESH_BINARY_INV);

    SimpleBlobDetector::Params params;


    params.filterByColor = true;
    params.blobColor = 0;

    params.filterByCircularity = true;
    params.minCircularity = 0.25;

    params.filterByConvexity = true;
    params.minConvexity = 0.5;

    params.filterByInertia = true;
    params.minInertiaRatio = 0.05;

    params.filterByArea = true;
    params.minArea = semaphoreMat.rows / 6 * semaphoreMat.cols / 6;
    params.maxArea = semaphoreMat.rows / 2 * semaphoreMat.cols / 3;

    Ptr<SimpleBlobDetector> blobDetector = SimpleBlobDetector::create(params);

    std::vector<cv::KeyPoint> keypoints;

    blobDetector->detect(semaphoreCopy, keypoints);


    //namedWindow("semaphoreMophologyMat", WINDOW_NORMAL);
    //imshow("semaphoreMophologyMat", semaphoreCopy);

    Mat semaphoreCopyKeypoints;

    drawKeypoints(semaphoreCopy, keypoints, semaphoreCopyKeypoints, Scalar(0, 0, 255));
    //namedWindow("semaphoreCopyKeypoints", WINDOW_NORMAL);
    //imshow("semaphoreCopyKeypoints", semaphoreCopyKeypoints);

    return keypoints;
}

const string enumToString(RULE_TYPE ruleType)
{

    const std::map<RULE_TYPE, const std::string> EnumToStringMap
    {
        {SEMAPHORE,"SEMAPHORE"},
        {FORBIDDEN_FORWARD,"FORBIDDEN_FORWARD"},
        {FORBIDDEN_ON_THE_LEFT,"FORBIDDEN_ON_THE_LEFT"},
        {FORBIDDEN_FORWARD_OR_ON_THE_LEFT,"FORBIDDEN_FORWARD_OR_ON_THE_LEFT"},
        {FORBIDDEN_ON_THE_RIGHT,"FORBIDDEN_ON_THE_RIGHT"},
        {FORBIDDEN_FORWARD_OR_ON_THE_RIGHT,"FORBIDDEN_FORWARD_OR_ON_THE_RIGHT"}
    };

    auto it = EnumToStringMap.find(ruleType);
    return it==EnumToStringMap.end() ? "Out of range": it->second;


}
}
