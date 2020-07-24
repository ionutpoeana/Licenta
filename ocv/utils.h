#pragma once

#include "constants.h"
#include "component.h"

#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>


using namespace std;
namespace cv
{

void showFrame(std::string name,cv::Mat &frame, WindowFlags flag);

// checks if there is motin in a matrix
// it counts the non zero values
bool isMotionInMat(Mat mat);

// takes in an opencv typecv::Mat and creates a matrix of char values
// if there is movement in cv::Mat then it marks the position
// in motionMatrix with 1
void processMotionMat(Mat &mat, vector<vector<long long>>& motionMatrix);

// retruns the openxc stylecv::Mat form a char type matrix
Mat getMotionMat(vector<vector<long long>> motionMatrix);

// orders 4 points in a polygon with maximum area
// the order is clockwise starting with topLeft
vector<Point> orderContourPoints(vector<Point> contour);

// alocates memory and intializes motion matrix with 0
vector<vector<long long>> alocateMotionMatrix(Rect rectangle);

// callback function used to get a 4 Point contour from an image
void getContourMouseCbk(int event, int x, int y, int, void* params);

// returns a 4 points contour starting with topLeft in clockwise order
vector<Point> getObjectContour(string frameName,cv::Mat frameMat, Rect& rect);

// crops and returns a interior croped matrix
Mat cropObject(Mat mat, vector<vector<Point>> contour);

// returns a matrix with all conected components
list<Component> getComponents(vector<vector<long long>> &motionMatrix);

// matches the current frame components with the previous ones
// it uses kalman filter for trajectory prediction
void matchCurrentFrameComponenets(std::list<Component>& currentFrameComponents, std::list<Component>& components);

// gets the contour area of which the semaphore does apply
vector<Point>getSemaphoreDelimiterArea(String frameName,cv::Mat frame);

// checks if a specific light is on in a specific circle
bool isCircleLightOn(Mat semaphoreMat, KeyPoint circle, SEMAPHORE_LIGHT light);

// gets the keypoints of blobs detected
// the blobs are filtered by circularity, position, radius
std::vector<cv::KeyPoint> getKeypoints(Mat semaphoreMat);

const std::string enumToString(RULE_TYPE ruleType);

}
