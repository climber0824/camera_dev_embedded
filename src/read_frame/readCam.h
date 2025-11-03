#pragma once
#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

class Camera {
private:
    std::string source;
    std::string windowName;
    cv::VideoCapture cap;
    int time_sleep = 10;
    float motion_thresh = 3.0f;   // percentage threshold
    bool running;
    bool show_image = false;
    bool save_image = true;

    std::string getTimestampFilename(const std::string camera_type);
    cv::Mat edgeDetection(const cv::Mat& origin_frame);
    bool detectMotion(const cv::Mat& prev, const cv::Mat& curr, float motion_thresh);

public:
    Camera(const std::string& src, const std::string& winName);
    bool open();
    void process();
};

