/*
This is a file implementing some image enhancements.
The common pipeline is:
1. Contrast Enhancement (to set the base dynamic range)

2. Saturation Boost (to pop the colors)

3. Sharpening (as a final step to enhance details)

cimpile: 
g++ display_enhancement.cpp -o display_enhancement $(pkg-config --cflags --libs opencv4)
*/
#include "DisplayEnhancement.h"


DisplayEnhancement::DisplayEnhancement() {}

DisplayEnhancement::~DisplayEnhancement() {}

/**
 * @brief 1. Enhances contrast using alpha (contrast) and beta (brightness).
 * This is the simplest "contrast/brightness" slider.
 * out = alpha * in + beta
 */

cv::Mat DisplayEnhancement::enhanceContrast(const cv::Mat &src, double alpha, int beta) {
    cv::Mat enhanced_img;
    
    // -1 for output type means the same as the input (src.type())
    src.convertTo(enhanced_img, -1, alpha, beta);

    return enhanced_img;
}


/**
 * @brief 2. Sharpens an image using the "Unsharp Mask" (USM) algorithm.
 * Sharpened = Original + (Original - Blurred) * amount
 */

cv::Mat DisplayEnhancement::enhanceSharpness(const cv::Mat &src, double amount) {
    cv::Mat enhanced_img;
    cv::Mat blurred;

    cv::GaussianBlur(src, blurred, cv::Size(0, 0), 3);

    // Add the "detail" (Original - Blurred) back to the original
    // This is the C++ equivalent of the formula
    cv::addWeighted(src, 1.0 + amount, blurred, -amount, 0, enhanced_img);

    return enhanced_img;
}


/**
 * @brief 3. Boosts color saturation using the HSV color space.
 * We convert to HSV, scale the 'S' channel, and convert back.
 */

cv::Mat DisplayEnhancement::enhanceSaturation(const cv::Mat &src, double factor) {
    cv::Mat hsv_img;
    // convert from BGR to HSV
    cv::cvtColor(src, hsv_img, cv::COLOR_BGR2HSV);

    // split 3 channels into seperate images
    std::vector<cv::Mat> channels;
    cv::split(hsv_img, channels);

    // channels[0] = Hue (the color)
    // channels[0] = Saturation (the intensity of the color)
    // channels[0] = Value (the brightness)

    // scale the Saturation channel
    channels[1].convertTo(channels[1], -1, factor, 0);

    // ensure the value <= 255
    cv::threshold(channels[1], channels[1], 255, 255, cv::THRESH_TRUNC);

    // merge the modified channels back together
    cv::Mat enhanced_hsv;
    cv::merge(channels, enhanced_hsv);

    // convert back from HSV to BGR for display
    cv::Mat enhanced_img;
    cv::cvtColor(enhanced_hsv, enhanced_img, cv::COLOR_HSV2BGR);

    return enhanced_img;
}


int main(int argc, char** argv) {
    std::string filename = "input_img.png";     // default name
    if (argc > 1) {
        filename = argv[1];
    }

    cv::Mat img = cv::imread(filename);
    if (img.empty()) {
        std::cerr << "Error: could not load image: " << filename << std::endl;
        return 1;
    }

    // apply contract enhancement
    // alpha > 1.0: increase contrast, beta = brightness
    cv::Mat contrast_img = enhanceContrast(img, 1.2, 10);
    cv::imwrite("contrast.png", contrast_img);

    // apply sharpening
    // a small amount (0.5 ~ 1.5) is usually best
    cv::Mat sharp_img = enhanceSharpness(img, 0.8);
    cv::imwrite("sharpness.png", sharp_img);

    // apply saturation boost
    // 1.0: no change, 1.5: 50% more saturation
    cv::Mat saturation_img = enhanceSaturation(img, 1.5);
    cv::imwrite("saturation.png", saturation_img);

    std::cout << "Image enhancements complete." << std::endl;

    return 0;
}