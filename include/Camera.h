#ifndef CAMERA_H
#define CAMERA_H

#include <string.h>
#include <memory>
#include <opevcv2/opencv.hpp>


// 3A setting structures
struct AESettings {
    bool autoExposure;
    double exposure;                // exposure time (-13 to -1 for log scale)
    double targetBrightness;        // target brightness (0-255)    
    double exposureCompensation;    // EV compensation (-2.0 to 2.0)
};


struct AWBSettings {
    bool autoWhiteBalance;
    double colorTemperature;        // Kelvin (2000 - 10000)
    double redGain;                 // Red channel gain (0.0 - 4.0)
    double blueGain;                // Blue channel gain (0.0 - 4.0)
};


struct AFSettings{
    bool autoFocus;
    int focusPosition;              // Focus position (0 - 255)
    double focusScore;              // Current focus quality score
};



// abstract base class for all camera types
class Camera {
protected:
    std::string id;
    std::string name;
    bool isConnected;
    cv::Mat currFrame;
    int width;
    int height;
    int fps;

    // 3A settings
    AESettings aeSettings;
    AWBSettings awbSettings;
    AFSettings afSettings;

    // 3A state
    cv::Mat prevFrame;
    std::vector<double> exposureHistory;
    std::vector<double> brightnessHistory;
    int maxHistorySize;

public:
    Camera(const std::string &id, const std::string &name);
    virtual ~Camera();

    // pure virtual functions
    virtual bool connect() = 0;
    virtual bool disconnect() = 0;
    virtual bool captureFrame() = 0;
    virtual bool isAvailable() const = 0;

    // common functions
    cv::Mat getFrame() const;
    std::string getId() const;
    std::string getName() const;
    bool getConnectStatus() const;
    void setResolution(int width, int height);
    void setFPS(int fps);


    // ===== 3A tuning functions ===== //
    
    // auto exposure (AE)
    void enableAutoExposure(bool enable);
    bool tuneAutoExposure();
    double calculateFrameBrightness(const cv::)
    double calculateOptimalExposure(double currBrightness, double targetBrightness);
    void setExposure(double exposure);
    void setTargetBrightness(double brightness);
    void setExposureCompensation(double compensation);
    AESettings getAESettings() const;

    // auto white balance (AWB)
    void enableAutoWhiteBalance(bool enable);
    bool tuneAutoWhiteBalance();
    void estimateColorTemperature(const cv::MAT &frame, double &temp, double &redGain, double &blueGain);
    void applyWhiteBalance(cv::Mat &frame);
    void setWhiteBalanceGains(double redGain, double blueGain);
    void setColorTemperature(double temperature);
    AWBSettings getAWBSettings() const;

    // auto focus (AF)
    void enableAutoFocus(bool enable);
    bool tuneAutoFocus();
    double calculateFocusScore(const cv::Mat &frame);
    int findOptimalFocus();
    void setFocusPosition(int position);
    AFSettings getAFSettings() const;

    // combined 3A tuning
    bool run3ATuning();
    void reset3ASettings();

protected:
    // helper functions
    void updateHistory(std::vector<double> &history, double value);
    double calculateMovingAverage(const std::vector<double> &history, int window);
    cv::Mat convertToGray(const cv::Mat &frame);
};

#endif