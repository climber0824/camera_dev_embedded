#ifndef SURVEILLANCE_SYSTEM_H
#define SURVEILLANCE_SYSTEM_H

#include <map>
#include <thread>
#include <atomic>

#include "CameraManager.h"
#include "MotionDetector.h"
#include "VideoRecorder.h"


class SurveillanceSystem {
private:
    CameraManager camManager;
    std::map<std::string, MotionDetector> motionDetectors;
    std::map<std::string, VideoRecorder> videoRecorders;

    std::atomic<bool> running;
    std::map<std::string, std::thread> monitorThreads;

    void monitorCamera(const std::string &cameraId);

public:
    SurveillanceSystem();
    ~SurveillanceSystem();

    // camera management
    bool addCamera(std::shared_ptr<Camera> camera);
    bool removeCamera(const std:: &id);

    // motion detection
    bool enableMotionDetection(const std::string &camId, int threshold=25);
    bool disableMotionDetection(const std::string &camId);

    // recording
    bool startRecording(const std::string &camId, const std::string &filename);
    bool stopRecording(const std::string &camId);

    // system control
    bool start();
    bool stop();
    bool isRunning const();

    // display
    void displayCamera(const std::string &camId);
    void displayAllCameras();
}

#endif