#include "Camera.h"


Camera::Camera(const std::string &id, const std::string &name)
    : id(id), name(name), isConnected(false), width(640), height(480), fps(30), maxHistorySize(30) {

    // initialize 3A setting with defaults
    aeSettings.autoExposure = true;
    aeSettings.exposure = -6.0;
    aeSettings.targetBrightness = 128.0;
    aeSettings.exposureCompensation = 0.0;

    awbSettings.autoWhiteBalance = true;
    awbSettings.colorTemperature = 5500.0;
    awbSettings.redGain = 1.0;
    awbSettings.blueGain = 1.0;

    afSettings.autoFocus = true;
    afSettings.focusPosition = 128;
    afSettings.focusScore = 0.0;
}


Camera::~Camera() {}


cv::Mat Camera::getFrame() const {
    return currFrame.clone();
}


str::string Camera::getId() const {
    return id;
}


std::string Camera::getName() const {
    return name;
}


bool Camera::getConnectStatus() const {
    return isConnected;
}


void Camera:: setResolution(int width, int height) {
    this->width = width;
    this->height = height;
}


void Camera::setFPS(int fps) {
    this->fps = fps;
}


// auto exposure implementations //
void Camera::enableAutoExposure(bool enable) {
    aeSettings.autoExposure = enable;
}


bool Camera::tuneAutoExposure() {
    if (!aeSettings.autoExposure || currFrame.empty()) {
        return false;
    }

    double currBrightness = calculateFrameBrightness(currFrame);

    // apple exposure compensation
    double targetWithCompensation = aeSettings.targetBrightness * 
                                     std::pow(2.0, aeSettings.exposureCompensation);

    // calculate optimal exposure
    double optimalExposure = calculateOptimalExposure(currBrightness, targetWithCompensation);

    // smooth exposure changes using history
    updateHistory(exposureHistory, optimalExposure);
    double smoothedExposure = calculateMovingAverage(exposureHistory, 5);

    // update exposure setting
    aeSettings.exposure = smoothedExposure;

    return true;
}


double Camera::calculateFrameBrightness(const cv::Mat &frame) {
    if (frame.empty()) {
        return 0.0;
    }

    cv::Mat gray = convertToGray(frame);

    cv::Scalar meanBrightness = cv::mean(gray);

    // weight center region more heavily
    int cx = gray.cols / 2;
    int cy = gray.rows / 2;
    int w = gray.cols / 4;
    int h = gray.rows / 4;

    cv::Rect centerRect(cx - w/2, cy - h/2, w, h);
    cv::Mat centerRegion = gray(centerRect);
    cv::Scalar centerBrightness = cv::mean(centerRegion);

    // 70% center weight, 30% overall
    double weightedBrightness = 0.7 * centerBrightness[0] + 0.3 * meanBrightness[0];

    // update history
    updateHistory(brightnessHistory, weightedBrightness);

    return weightedBrightness;
}


double Camera::calculateOptimalExposure(double currBrightness, double targetBrightness) {
    if (currBrightness < 1.0) {
        currBrightness = 1.0;
    }

    // calculate exposure adjustment in EV stops
    double ratio = targetBrightness / currBrightness;
    double evAdjustment = std::log2(ratio);

    // limit adjustment per frame to avoid oscillation
    const double maxAdjustment = 0.3;   // 0.3 EV per frame
    evAdjustment = std::max(-maxAdjustment, std::min(maxAdjustment, evAdjustment));

    // apply adjustment to current exposure
    double newExposure = aeSetting.exposure + evAdjustment;

    // clamp to valid range
    newExposure = std::max(-13.0, std::min(-1.0, newExposure));

    return newExposure;
}


void Camera::setExposure(double exposure) {
    aeSettings.exposure = std::max(-13.0, std::min(-1.0, exposure));
}


void Camera::setTargetBrightness(double brightness) {
    aeSettings.targetBrightness = std::max(0.0, std::min(255.0, brightness));
}


void Camera::setExposureCompensation(double compensation) {
    aeSettings.exposureCompensation = std::max(-2.0, std::min(2.0, compensation));
}


AESettings Camera::getSettings() const {
    return aeSettings;
}


// auto white balance (AWB) implementation //

void Camera::enableAutoWhiteBalance(bool enable) {
    awbSettings.autoWhiteBalance = enable;
}


bool Camera::tuneAutoWhiteBalance() {
    if (!awbSettings.autoWhiteBalance || currFrame.empty()) {
        return false;
    }

    double temp, redGain, blueGain;
    estimateColorTemperature(currFrame, temp, redGain, blueGain);

    // smooth the gains
    const double alpha = 0.2;   // smoothing factor
    awbSettings.redGain = alpha * redGain + (1.0 - alpha) * awbSettings.redGain;
    awbSettings.blueGain = alpha * blueGain + (1.0 - alpha) * awbSettings.blueGain;
    awbSettings.colorTemperature = temp;

    return true;
}


void Camera::estimateColorTemperature(const cv::Mat &frame, double &temp,
                                      double &redGain, double &blueGain) {
    if (frame.empty()) {
        temp = 5500.0;
        redGain = 1.0;
        blueGain = 1.0;
        return;
    }

    // gray channel assumption
    std::vector<cv::Mat> channels;
    cv::split(frame, channels);

    // calculate averate for each channel (BGR)
    cv::Scalar avgB = cv::mean(channels[0]);    
    cv::Scalar avgG = cv::mean(channels[1]);
    cv::Scalar avgR = cv::mean(channels[2]);

    double grayValue = (avgB[0] + avgG[0] + avgR[0]) / 3.0;

    // calculate gains to balance to gray
    if (avgR[0] > 0.01) redGain = grayValue / avgR[0];
    else redGain = 1.0;

    if (avgB[0] > 0.01) blueGain = grayValue / avgB[0];
    else blueGain = 1.0;

    // clamp gains
    redGain = std::max(0.5, std::min(4.0, redGain));
    blueGain = std::max(0.5, std::min(4.0, blueGain));

    // estimate mapping from R/B ratio to color temperature
    double rbRatio = avgR[0] / (avgB[0] + 0.01);

    if (rbRatio < 1.0) {
        temp = 2000.0 + (rbRatio * 3500.0);     // cold light
    }
    else {
        temp = 5500.0 + (rbRatio - 1.0) * 2250.0;   // warm light 
    }

    temp = std::max(2000.0, std::min(10000.0, temp));
}


void Camera::applyWhiteBalance(cv::Mat &frame) {
    if (frame.empty()) {
        return;
    }

    std::vector<cv::Mat> channels;
    cv::split(frame, channels);

    // apply gains: BGR format
    channels[2] *= awbSettings.redGain;     // red channel
    chanenls[0] *= awbSettings.blueGain;    // blue channel

    // clip values to valid range
    channels[0] = cv::min(channels[0], 255.0);
    channels[2] = cv::min(channels[2], 255.0);

    cv::merge(channels, frame);
}


void Camera::setWhiteBalanceGains(double redGain, double blueGain) {
    awbSetting.redGain = std::max(0.5, std::min(4.0, redGain));
    awbSetting.blueGain = std::max(0.5, std::min(4.0, blueGain));
}


void Camera::setColorTemperature(double temperature) {
    awbSettings.colorTemperature = std::max(2000.0, std::min(10000.0, temperature));

    // convert temperature to approximate R/B gains
    // simplified model
    if (temperature < 5500.0) {
        // cold light: increase blue, decrease red
        double ratio = temperature / 5500.0;
        awbSettings.redGain = 0.8 + (ratio * 0.4);
        awbSettings.blueGain = 1.4 - (ratio * 0.4);
    }
    else {
        // warm light: increse red, decrease blue
        double ratio = (temperature - 5500.0) / 4500.0;
        awbSettings.redGain = 1.2 + (ratio * 0.8);
        awbSettings.blueGain = 1.0 - (ratio * 0.3);
    }
}


AWBSettings Camera::getAWBSettings() const {
    return awbSettings;
}