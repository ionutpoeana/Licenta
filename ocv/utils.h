#pragma once

#include "constants.h"
#include "component.h"

#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>


using namespace std;
using namespace cv;

void showFrame(std::string name,cv::Mat &frame, WindowFlags flag);

Mat computeHistogram(Mat frame);

// checks if there is motin in a matrix
// it counts the non zero values
bool isMotionInMat(Mat mat);


// takes in an opencv typecv::Mat and creates a matrix of char values
// if there is movement in a 10x10cv::Mat then it marks the position
// in motionMatrix with 1
void processMotionMat(Mat &mat, vector<vector<long long>>& motionMatrix);

// retruns the openxc stylecv::Mat form a char type matrix
Mat getMotionMat(vector<vector<long long>> motionMatrix);

//
bool isCrosswalkCrossedByCars(vector<vector<long long>> motionMatrix, vector<Point> contour, OBJECT_ORIENTATION orientation);


// orders 4 points in a polygon with maximum area
// the order is clockwise starting with topLeft
vector<Point> orderContourPoints(vector<Point> contour);

// alocates memory and intializes motion matrix with 0
vector<vector<long long>> alocateMotionMatrix(Rect rectangle);

// returns vertical semaphore light using intensity of BGR pixels
// in every third part of semaphore
SEMAPHORE_LIGHT getSemaphoreLight(Mat img);

// callback function used to get a 4 Point contour from an image
void getContourMouseCbk(int event, int x, int y, int, void* params);

// returns a 4 points contour starting with topLeft in clockwise order
vector<Point> getObjectContour(string frameName,cv::Mat frameMat, Rect& rect);

// crops and returns a interior croped matrix
Mat cropObject(Mat mat, vector<vector<Point>> contour);

// counts all non zero elements between 2 points in the matrix
int countWhiteBoxes(vector<vector<long long>> motionMatrix, int x, int y, int x2, int y2);

// returns a matrix with all conected components
list<Component> getComponents(vector<vector<long long>> &motionMatrix);

// resets all elements in the motion matrix to 0
void resetMotionMatrix(vector<vector<long long>>& motionMatrix);


void writeProofOfCrime(String name, queue<Mat>& proofOfCrime);

void matchCurrentFrameComponenets(std::list<Component>& currentFrameComponents, std::list<Component>& components);
// extras background
// creat blobi
// aplicat algoritm de tracking
// verificat limite

vector<Point>getSemaphoreDelimiterArea(String frameName,cv::Mat frame);

