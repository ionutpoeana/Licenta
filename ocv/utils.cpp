
#include "utils.h"



extern long long TIME;
extern int DISTANCE_THRESHOLD;


using namespace std;
using namespace cv;

Mat computeHistogram(Mat frame)
{
    int histSize = 256;
    float range[] = { 0,256 };
    const float* histRange = { range };
    Mat bHist, gHist, rHist;

    vector<Mat> bgrArrays;
    split(frame, bgrArrays);

    bool uniform = true;
    bool accumulate = false;

    calcHist(&bgrArrays[0], 1, 0, Mat(), bHist, 1, &histSize, &histRange, uniform, accumulate);
    calcHist(&bgrArrays[1], 1, 0, Mat(), gHist, 1, &histSize, &histRange, uniform, accumulate);
    calcHist(&bgrArrays[2], 1, 0, Mat(), rHist, 1, &histSize, &histRange, uniform, accumulate);

    int histWidth = 512;
    int histHeight = 400;

    int bin_w = cvRound(histWidth / 1.0 / histSize);

    Mat histImage(histHeight, histWidth, CV_8UC3, Scalar(0, 0, 0));

    normalize(bHist, bHist, 0, histImage.rows, NORM_MINMAX, -1);
    normalize(gHist, gHist, 0, histImage.rows, NORM_MINMAX, -1);
    normalize(rHist, rHist, 0, histImage.rows, NORM_MINMAX, -1);

    for (int i = 1; i < histSize; ++i)
    {
        // trag o linie intre valori consecutive ale numarului de pixeli de o anumita intensitate
        line(histImage, Point(bin_w * (i - 1), histHeight - cvRound(bHist.at<float>(i - 1))), Point(bin_w * i, histHeight - cvRound(bHist.at<float>(i))), Scalar(255, 0, 0));
        line(histImage, Point(bin_w * (i - 1), histHeight - cvRound(gHist.at<float>(i - 1))), Point(bin_w * i, histHeight - cvRound(gHist.at<float>(i))), Scalar(0, 255, 0));
        line(histImage, Point(bin_w * (i - 1), histHeight - cvRound(rHist.at<float>(i - 1))), Point(bin_w * i, histHeight - cvRound(rHist.at<float>(i))), Scalar(0, 0, 255));
    }

    return histImage;

}

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
    imshow(name, frame);
}

void processMotionMat(Mat& mat, vector<vector<long long>>& motionMatrix)
{
    Mat box;
    int lines = motionMatrix.size();
    int cols = motionMatrix[0].size();

    auto time = std::chrono::system_clock::now();

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



// trebuie sa realizez un vector cu centri de greutate
// centru de greutate = media coordonatelor i si j din matrice
// realizarea lor se face dupa calcularea componentelor conectate

// ar fi util sa fac o mediere pe numarul de pixeli din componente
// las sa treaca doar componentele care au un numar mai mare sau egal cu jumatatea mediei medierii


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
            if (motionMatrix[i - 1][j - 1] && motionMatrix[i - 1][j - 1] > (TIME - NORMAL_FRAME_TIME * MAX_FRAMES_NO_MOVEMENT_TIME))
            {
                componentMatrix[i][j] = 1;
            }
            else
            {
                componentMatrix[i][j] = 0;
            }
        }
    }


    unsigned int currentComponentonent = 0;
    unsigned int conflict[500] = { 0 };

    for (int i = 1; i < rows-1; ++i)
    {
        for (int j = 1; j < cols-1; ++j)
        {
            // daca avem miscare
            if (componentMatrix[i][j] != 0)
            {

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
                            ++currentComponentonent;
                            componentMatrix[i][j] = currentComponentonent;
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
    }

    Component* components = new Component[currentComponentonent+2];//(component*)malloc(sizeof(component)*(currentComponentonent+2));

    for (int i = 1; i <= currentComponentonent; ++i)
    {
        int conf = conflict[i];
        if (conf < 0) conf = 0;
        while (conflict[conf])
        {
            conf = conflict[conf];
        }
        conflict[i] = conf;
    }

    for (int i = 1; i < rows; ++i)
    {
        for (int j = 1; j < cols - 1; ++j)
        {
            if (componentMatrix[i][j])
            {
                if (conflict[componentMatrix[i][j]])
                {
                    componentMatrix[i][j] = conflict[componentMatrix[i][j]];
                }

                // in our image the coordonate axis are changed
                components[componentMatrix[i][j]].center.x += j;
                components[componentMatrix[i][j]].center.y += i;
                ++components[componentMatrix[i][j]].nrPoints;
            }
        }
    }

    // take only the components over avg number of fixels
    int avgComponentPixels = 0;
    if (currentComponentonent > 0)
    {
        for (int i = 1; i <= currentComponentonent; ++i)
        {
            avgComponentPixels += components[i].nrPoints;
        }

        avgComponentPixels /= currentComponentonent;
    }

    list<Component> filteredComponents;

    for (int i = 1; i <= currentComponentonent; ++i)
    {
        if (components[i].nrPoints >= avgComponentPixels)
        {
            components[i].center.x /= components[i].nrPoints;
            components[i].center.y /= components[i].nrPoints;

            Component component;
            component.center.x = components[i].center.x * BOX_SIZE + BOX_SIZE / 2;
            component.center.y = components[i].center.y * BOX_SIZE + BOX_SIZE / 2;
            component.nrPoints = components[i].nrPoints;
            filteredComponents.push_back(component);
        }
    }

    for (int i = 0; i < rows; ++i)
    {
        free(componentMatrix[i]);
    }
    free(componentMatrix);
    delete[]components;
    return filteredComponents;
}

bool isCrosswalkCrossedByCars(vector<vector<long long>> crosswalkMotion, vector<Point> contour, OBJECT_ORIENTATION orientation)
{

    int startLineBoxes = 0;
    int finishLineBoxes = 0;
    int topIndex = crosswalkMotion.size() - 1;
    double startLine = 0;
    double  finishLine = 0;
    int startX, stopX;

    Point topLeft = contour[0];
    Point topRight = contour[1];
    Point bottomRight = contour[2];
    Point bottomLeft = contour[3];
    vector<Point> conturParcurs;

    switch (orientation)
    {
        // in opencv 00 is the top left
        // rows/cols is the bottom right
        // the matrix grows in third quadrant
        // fa abstractie la matrice ca fiind oglindita peste
    case OBJECT_ORIENTATION::VERTICAL:
        startLineBoxes = countWhiteBoxes(crosswalkMotion, bottomLeft.x / BOX_SIZE, bottomLeft.y / BOX_SIZE, topLeft.x / BOX_SIZE, topLeft.y / BOX_SIZE);
        finishLineBoxes = countWhiteBoxes(crosswalkMotion, topRight.x / BOX_SIZE, topRight.y / BOX_SIZE, bottomRight.x / BOX_SIZE, bottomRight.y / BOX_SIZE);
        break;
    case OBJECT_ORIENTATION::HORIZONTAL:

        finishLineBoxes = countWhiteBoxes(crosswalkMotion, bottomLeft.x / BOX_SIZE, bottomLeft.y / BOX_SIZE, bottomRight.x / BOX_SIZE, bottomRight.y / BOX_SIZE);
        startLineBoxes = countWhiteBoxes(crosswalkMotion, topLeft.x / BOX_SIZE, topLeft.y / BOX_SIZE, topRight.x / BOX_SIZE, topRight.y / BOX_SIZE);

        break;
    default:
        break;
    }

    if (startLineBoxes >= MIN_BOX_NUMBER && finishLineBoxes >= MIN_BOX_NUMBER)
    {
        return true;
    }

    return false;
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

void resetMotionMatrix(vector<vector<long long>>& motionMatrix)
{
    for (auto& row : motionMatrix)
    {
        for (auto& box : row)
        {
            box = 0;
        }
    }
}

void writeProofOfCrime(String name, queue<Mat>& proofOfCrime)
{
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
    VideoWriter videoWritter(name, codec, 30, proofOfCrime.front().size(), true);
    Mat proof;
    for (; !proofOfCrime.empty();)
    {
        proof = proofOfCrime.front();
        proofOfCrime.pop();
        videoWritter.write(proof);
    }
    videoWritter.release();
}



void matchCurrentFrameComponenets(list<Component>& currentFrameComponents, list<Component>& components)
{
    // trebuie sa fac match intre componente
    // daca gasesc un match adaug centru componentei actuale la vectorul istoric si centru devine centrul noii componente
    for (auto& comp : components)
    {
        comp.matchFound = false;
    }

    double currentDistance;
    double minDistance;
    bool matchFound;
    Mat predictionMat;
    Mat measurementMat = Mat::zeros(MEASURE_NUM, 1, CV_32F);
    Point2f predictedPoint, closestPointEstimation;
    Point2f statePoint;
    for (auto& currentComponent : currentFrameComponents)
    {
        minDistance = DISTANCE_THRESHOLD;
        Component* componentCopy = nullptr;
        Component* matchComponent = nullptr;

        matchFound = false;

        // fac predict pe trackerul din components
        // fac correct pe match-ul gasit
        // fac update pe trackerul din components

        // trebuie sa fac odata predict

        for (auto& comp : components)
        {
            // predict where next component center will be


            // calculate norm between predicted center and new found components
            currentDistance = norm(currentComponent.center - comp.center);
            if ((currentDistance < minDistance) && !comp.matchFound && !currentComponent.matchFound)
            {

                predictionMat = comp.kf.predict();
                predictedPoint = Point2f(predictionMat.at<float>(0, 0), predictionMat.at<float>(1, 0));
                closestPointEstimation = norm(currentComponent.center - comp.center) < norm(comp.center - predictedPoint) ? currentComponent.center : predictedPoint;

                minDistance = currentDistance;
                componentCopy = &comp;
                matchComponent = &currentComponent;
                matchFound = true;
            }
        }

        if (matchFound)
        {
            // measurement is the point with smallest distance between currentComponenet and component
            measurementMat.at<float>(0, 0) = matchComponent->center.x;
            measurementMat.at<float>(1, 0) = matchComponent->center.y;

            componentCopy->kf.correct(measurementMat);

            // update history vector
            componentCopy->centerHistory.push_back(componentCopy->center);

            // update the component center with predictedPoint
            componentCopy->center = closestPointEstimation;
            componentCopy->matchFound = true;
            matchComponent->matchFound = true;
            componentCopy->nrNoDetectionFrames = 0;
        }
        else if (componentCopy && matchComponent)
        {
            componentCopy->matchFound = false;
            matchComponent->matchFound = false;
        }
    }

    for (auto comp = components.begin(); comp != components.end();)
    {
        if (!comp->matchFound)
        {
            comp->nrNoDetectionFrames++;
        }

        if (comp->nrNoDetectionFrames > MAXIMUM_UNDETECTED_NR_FRAMES)
        {
            comp = components.erase(comp);
        }
        else
        {
            ++comp;
        }
    }

    for (auto currentComponent : currentFrameComponents)
    {
        if (!currentComponent.matchFound)
        {
            currentComponent.componentIndex = Component::index;
            ++Component::index;
            components.push_front(currentComponent);
        }
    }
    // daca nu gasesc un match adaug noile componenete
    // daca am componente care au trecut de un numar de frame-uri fara un match, le elimin
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
    showFrame(frameName, frame, WINDOW_NORMAL);
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

        polylines(frameClone, delimiterLine, true, Scalar(0, 0, 255), 4);
        showFrame(frameName, frameClone, WINDOW_NORMAL);

        fprintf(stdout, "Apastati SPACE pentru a valida selectia! ESC pentru a selecta din nou!\n ");
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



SEMAPHORE_LIGHT getSemaphoreLight(Mat img)
{
    Vec3b* ptr = img.ptr<Vec3b>();
    int redPixels = 0;
    int greenPixels = 0;
    int yellowPixels = 0;

    for (int i = 0; i < img.rows / 3; ++i)
    {
        ptr = img.ptr<Vec3b>(i);
        for (int j = 0; j < img.cols; ++j)
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

    for (int i = img.rows / 3; i < 2 * img.rows / 3; ++i)
    {
        ptr = img.ptr<Vec3b>(i);
        for (int j = 0; j < img.cols; ++j)
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

    for (int i = 2 * img.rows / 3; i < img.rows; ++i)
    {
        ptr = img.ptr<Vec3b>(i);
        for (int j = 0; j < img.cols; ++j)
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

    if (redPixels > greenPixels && redPixels > yellowPixels)
    {
        return SEMAPHORE_LIGHT::RED;
    }

    if (greenPixels > redPixels && greenPixels > yellowPixels)
    {
        return SEMAPHORE_LIGHT::GREEN;
    }

    if (yellowPixels > redPixels && yellowPixels > greenPixels)
    {
        return SEMAPHORE_LIGHT::YELLOW;
    }
    return SEMAPHORE_LIGHT::UNDEFINED;
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
        fprintf(stdout, "Alegeti 4 puncte de incadrare a trecerii de pietoni!\n");
        showFrame(frameName, frameClone, WINDOW_NORMAL);

        setMouseCallback(frameName, getContourMouseCbk, params);

        while (waitKey() != SPACE_KEY);

        polylines(frameClone, contour, true, Scalar(0, 0, 255), 4);
        showFrame(frameName, frameClone, WINDOW_NORMAL);

        fprintf(stdout, "Apastati SPACE pentru a valida selectia! ESC pentru a selecta din nou!\n ");
        int option = waitKey();

        if (option == SPACE_KEY)
        {
            rect = boundingRect(contour);

            destroyWindow(frameName);
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


int countWhiteBoxes(vector<vector<long long>> motionMatrix, int x, int y, int x2, int y2)
{

    if (y == motionMatrix.size()) --y;
    if (y2 == motionMatrix.size()) --y2;
    if (x == motionMatrix[0].size()) --x;
    if (x2 == motionMatrix[0].size()) --x2;

    int w = x2 - x;
    int h = y2 - y;

    int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;


    /*Rect rect;
    rect.x = x;
    rect.y = y;
    rect.width = w;
    rect.height = h;
    printf("Valori de inceput x si y\n");
    printf("X = %d, y = %d\n", x, y);
    printf("X = %d, y = %d\n", x2, y2);
    printf("\n\n");
    printf("marime matrce x= %d, y = %d", motionMatrix[0].size(), motionMatrix.size());*/

    if (w < 0) dx1 = -1; else if (w > 0) dx1 = 1;
    if (h < 0) dy1 = -1; else if (h > 0) dy1 = 1;
    if (w < 0) dx2 = -1; else if (w > 0) dx2 = 1;

    int longest = abs(w);
    int shortest = abs(h);

    int whiteBoxes = 0;

    if (!(longest > shortest)) {
        longest = abs(h);
        shortest = abs(w);
        if (h < 0)
            dy2 = -1;
        else if (h > 0)
            dy2 = 1;
        dx2 = 0;
    }

    //vector<vector<Point>> conturParcurs;
    //conturParcurs.push_back(vector<Point>());
    int numerator = longest >> 1;
    for (int i = 0; i <= longest; i++) {
        //cout << "(" << x << "," << y << ")\n";

        //conturParcurs[0].push_back(Point(x, y));
        if (motionMatrix[y][x])
        {
            ++whiteBoxes;
        }

        numerator += shortest;
        if (numerator < longest) {
            x += dx2;
            y += dy2;
        }
        else {

            numerator -= longest;
            x += dx1;
            y += dy1;
        }
    }


    //Mat mat = Mat(motionMatrix.size(),motionMatrix[0].size(), CV_8U);
    //polylines(mat, conturParcurs[0], false, Scalar::all(255), 1);
    //showFrame("contur parcurs", mat, WINDOW_NORMAL);
    //while (waitKey() != SPACE_KEY)
    //{

    //}
    return whiteBoxes;
}

// incerc sa creez o matrice de miscare pe baza unui timp 0, de la care pornesc programul
// afisez acea matrice de miscare -> kinda done, nu stiu sigur daca am nevoie  de matricea de miscare



