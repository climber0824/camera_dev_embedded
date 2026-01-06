#include <iostream>
#include <chrono>

#include "SurveillanceSystem.h"


SurveillanceSystem::SurveillanceSystem() : running(false) {}


SurveillanceSystem::~SurveillanceSystem() {
    stop();
}


bool SurveillanceSystem::addCamera(std::shared_ptr<Camera> cam) {
    if (!cam) {
        return false;
    }

    std::string id = cam->getId();

    // add to camera manager
    if (!camManager.addCamera(cam)) {
        return false;
    }

    // initialize motion detector for this camera
    motionDetectors[id] = MotionDetector(25, 500.0);

    std::cout << "Camera " << id << " added to surveillance system" << std::endl;

    return true;
}


bool SurveillanceSystem::removeCamera(const std::string &id) {
    // stop recording if active
    stopRecording(id);

    // remove from motion detectors
    motionDetectors.erase(id);

    // remove from recorders
    videoRecorders.erase(id);

    // remove from camera manager
    return camManager.removeCamera(id);
}


bool SurveillanceSystem::enableMotionDetection(const std::string &camId, int threshold) {
    auto cam = camManager.getCamera(camId);
    if (!cam) {
        std::cerr << "Camera not found: " << camId << std::endl;
        return false;
    }

    auto it = motionDetectors.find(camId);
    if (it != motionDetectors.end()) {
        it->second.setThreshold(threshold);
        std::cout << "Motion detection enabled for camera: " << camId << std::endl;
        return true;
    }

    return false;
}


bool SurveillanceSystem::disableMotionDetection(const std::string &camId) {
    auto it = motionDetectors.find(camId);
    if (it != motionDetectors.end()) {
        it->second.reset();
        std::cout << "Motion detection disabled for camera: " << camId << std::endl;
        return true;
    }
    return false;
}


bool SurveillanceSystem::startRecording(const std::string &camId, const std::string &filename) {
    auto cam = camManager.getCamera(camId);
    if (!cam) {
        std::cerr << "Camera not found: " << camId << std::endl;
        return false;
    }

    // create or get recorder
    if (videoRecorders.find(camId) == videoRecorders.end()) {
        videoRecorders[camId] = VideoRecorder();
    }

    // get frame size from camera (use default if not available)
    cv::Size frameSize(640, 480);

    return videoRecorders[camId].startRecording(filename, 30, frameSize);
}


bool SurveillanceSystem::stopRecording(const std::string &camId) {
    auto it = videoRecorders.find(camId);
    if (it != videoRecorders.end()) {
        return it->second.stopRecording();
    }
    return false;
}


void SurveillanceSystem::monitorCamera(const std::string &camId) {
    auto cam = camManager.getCamera(camId);
    if (!cam)
        return;

    // connect camera
    if (!cam->connect()) {
        std::cerr << "Failed to connect camera: " << camId << std::endl;
        return;
    }

    std::cout << "Monitoring started for camera: " << camId << std::endl;

    while (running) {
        // capture frame
        if (!cam->captureFrame()) {
            std::cerr << "Failed to capture frame from: " << camId << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // run 3A tuning
        cam->run3ATuning();

        // get current frame
        cv::Mat frame = cam->getFrame();
        if (frame.empty()) {
            continue;
        }

        // check for motion
        auto motionItem = motionDetectors.find(camId);
        if (motionItem != motionDetectors.end()) {
            bool motionDetected = motionItem->second.detectMotion(frame);

            if (motionDetected) {
                std::cout << "Motion detected on camera: " << camId << std::endl;

                // get motion region
                auto regions = motionItem->second.getMotionRegions(frame);

                // draw bounding box on frame
                for (const auto &rect : regions) {
                    cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);                    
                }                
            }
        }

        // record frame if recording is active
        auto recorderItem = videoRecorders.find(camId);
        if (recorderItem != videoRecorders.end() && recorderItem->second.getRecordingStatus()) {
            recorderItem->second.writeFrame(frame);
        }

        // small delay to control frame rate
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    cam->disconnect();
    std::cout << "Monitoring stopped for camera: " << camId << std::endl;

}

bool SurveillanceSystem::start() {
    if (running) {
        std::cout << "Surveillance system already running" << std::endl;
        return false;
    }

    running = true;

    // start monitoring thread for each camera
    auto cameras = camManager.getAllCameras();
    for (const auto &cam : cameras) {
        std::string id = cam->getId();
        monitorThreads[id] = std::thread(&SurveillanceSystem::monitorCamera, this, id);
    }

    std::cout << "Surveillance system started with " << cameras.size() << " cameras" << std::endl;

    return true;
}


bool SurveillanceSystem::stop() {
    if (!running) {
        return true;
    }

    running = false;

    // wait for all monitoring threads to finish
    for (auto &element : monitorThreads) {
        if (element.second.joinable()) {
            element.second.join();
        }
    }

    monitorThreads.clear();

    // stop all recordings
    for (auto &element : videoRecorders) {
        element.second.stopRecording();
    }

    std::cout << "Surveillance system stopped" << std::endl;

    return true;
}

bool SurveillanceSystem::isRunning() const {
    return running;
}

void SurveillanceSystem::displayCamera(const std::string &camId) {
    auto cam = camManager.getCamera(camId);
    if (!cam) {
        std::cerr << "camera not found: " << camId << std::endl;
        return; 
    }

    std::string windowName = "Camera: " + camId;
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);

    while (running) {
        cv::Mat frame = cam->getFrame();
        if (!frame.empty()) {
            // draw motion regions if motion detector exists
            auto motionIt = motionDetectors.find(camId);
            if (motionIt != motionDetectors.end()) {
                auto regions = motionIt->second.getMotionRegions(frame);
                for (const auto &rect : regions) {
                    cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);
                }
            }

            cv::imshow(windowName, frame);
        }

        if (cv::waitKey(30) == 'q') {
            break;
        }
    }

    cv::destroyWindow(windowName);
}


void SurveillanceSystem::displayAllCameras() {
    auto cameras = camManager.getAllCameras();

    std::vector<std::string> windowNames;
    for (const auto &cam : cameras) {
        std::string windowName = "Camera: " + cam->getId();
        cv::namedWindow(windowName, cv::WINDOW_NORMAL);
        windowNames.push_back(windowName);
    }

    while (running) {
        for (size_t i = 0; i < cameras.size(); i++) {
            cv::Mat frame = cameras[i]->getFrame();

            if (!frame.empty()) {
                std::string camId = cameras[i]->getId();

                // draw motion regions
                auto motionIt = motionDetectors.find(camId);
                if (motionIt != motionDetectors.end()) {
                    auto regions = motionIt->second.getMotionRegions(frame);
                    for (const auto &rect : regions) {
                        cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);
                    }
                }
                cv::imshow(windowNames[i], frame);
            }
        }

        // break using 'q' key
        if (cv::waitKey(30) == 'q') {
            break;
        }
    }

    for (const auto &windowName : windowNames) {
        cv::destroyWindow(windowName);
    }
}
