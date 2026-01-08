#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <functional>


class MotionDetector {
private:
    cv::Mat prevFrame;
    int threshold;
    double minArea;
    bool initialized;

public:
    MotionDetector(int threshold = 25, double minArea = 500.0);
    ~MotionDetector();

    bool detectMotion(const cv::Mat &currFrame);
    cv::Mat getMotionMask(const cv::Mat &currFrame);
    std::vector<cv::Rect> getMotionRegions(const cv::Mat &currFrame);

    void setThreshold(int threshold);
    void setMinArea(double area);
    void reset();
};

#endif