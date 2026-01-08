#include <iostream>

#include "MotionDetector.h"


MotionDetector::MotionDetector(int threshold, double minArea)
    : threshold(threshold), minArea(minArea), initialized(false) {}


MotionDetector::~MotionDetector() {}


bool MotionDetector::detectMotion(const cv::Mat &currFrame) {
    if (currFrame.empty()) {
        return false;
    }

    // convert to gray scale
    cv::Mat grayFrame;
    if (currFrame.channels() == 3) {
        cv::cvtColor(currFrame, grayFrame, cv::COLOR_BGR2GRAY);
    }
    else {
        grayFrame = currFrame.clone();
    }

    // apply Gaussian blur to reduce noise
    cv::GaussianBlur(grayFrame, grayFrame, cv::Size(21, 21), 0);

    // initialize previous frame on first run
    if (!initialized || prevFrame.empty()) {
        prevFrame = grayFrame.clone();
        initialized = true;

        return false;   // no motion on first frame
    }

    // compute absolute difference between curr and prev frame
    cv::Mat frameDelta;
    cv::absdiff(prevFrame, grayFrame, frameDelta);

    // apply threshold to get binary image
    cv::Mat thresh;
    cv::threshold(frameDelta, thresh, threshold, 255, cv::THRESH_BINARY);

    // dilate to fill in holes
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::dilate(thresh, thresh, kernel, cv::Point(-1, -1), 2);

    // find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // check if any contour is larger than  minium area
    bool motionDetected = false;
    for (const auto &c : contours) {
        double area = cv::contourArea(c);
        if (area >= minArea) {
            motionDetected = true;
            break;
        }
    }

    // update previous frame
    prevFrame = grayFrame.clone();

    return motionDetected;
}


cv::Mat MotionDetector::getMotionMask(const cv::Mat &currFrame) {
    if (currFrame.empty()) {
        return cv::Mat();
    }

    // convert to gray scale
    cv::Mat grayFrame;
    if (currFrame.channels() == 3) {
        cv::cvtColor(currFrame, grayFrame, cv::COLOR_BGR2GRAY);
    }
    else {
        grayFrame = currFrame.clone();
    }

    // apply Gaussian blur to reduce noise
    cv::GaussianBlur(grayFrame, grayFrame, cv::Size(21, 21), 0);

    // initialize if needed
    if (!initialized || prevFrame.empty()) {
        prevFrame = grayFrame.clone();
        initialized = true;

        return cv::Mat::zeros(currFrame.size(), CV_8UC1);
    }

    // compute absolute difference between curr and prev frame
    cv::Mat frameDelta;
    cv::absdiff(prevFrame, grayFrame, frameDelta);

    // apply threshold
    cv::Mat thresh;
    cv::threshold(frameDelta, thresh, threshold, 255, cv::THRESH_BINARY);

    // dilate to fill in holes
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::dilate(thresh, thresh, kernel, cv::Point(-1, -1), 2);    

    // update previous frame
    prevFrame = grayFrame.clone();

    return thresh;
}

std::vector<cv::Rect> MotionDetector::getMotionRegions(const cv::Mat &currFrame) {
    std::vector<cv::Rect> motionRegions;

    if (currFrame.empty()) {
        return motionRegions;
    }

    // get motion mask
    cv::Mat motionMask = getMotionMask(currFrame);

    if (motionMask.empty()) {
        return motionRegions;
    }

    // find contours in motion mask
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(motionMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // get bounding rectangles fpr contours larger than minium area
    for (const auto &c : contours) {
        double area = cv::contourArea(c);
        if (area >= minArea) {
            cv::Rect boundingRect = cv::boundingRect(c);
            motionRegions.push_back(boundingRect);
        }
    }

    return motionRegions;
}


void MotionDetector::setThreshold(int thresh) {
    this->threshold = std::max(0, std::min(255, thresh));
}


void MotionDetector::setMinArea(double area) {
    this->minArea = std::max(0.0, area);
}

void MotionDetector::reset() {
    prevFrame.release();
    initialized = false;
}
