#ifndef DISPLAY_ENHANCEMENT_H
#define DISPLAY_ENHANCEMENT_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>


class DisplayEnhancement {
public:
    DisplayEnhancement();
    ~DisplayEnhancement();

    cv::Mat enhanceContrast(const cv::Mat &src, double alpha, int beta);
    cv::Mat enhanceSharpness(const cv::Mat &src, double amount);
    cv::Mat enhanceSaturation(const cv::Mat &src, double factor);
}

#endif