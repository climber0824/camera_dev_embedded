#include <iostream>

#include "USBCamera.h"


USBCamera::USBCamera(const std::string &id, const std::string &name, int deviceIndex) 
    : Camera(id, name), deviceIndex(deviceIndex) {}


USBCamera::~USBCamera() {
    disconnect();
}

bool USBCamera::connect() {
    if (isConnected) {
        return true;
    }

    capture.open(deviceIndex);
    if (!capture.isOpen()) {
        std::cerr << "Failed to open USB camera " << deviceIndex << std::endl;
        return false;
    }

    capture.set(cv::CAP_PROP_FRAME_WIDTH, width);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    capture.set(cv::CAP_PROP_FRAME_FPS, fps);

    isConnected = true;
    std::cout << "USB cam " << name << " connected" << std::endl;
    
    return true;
}


bool USBCamera::disconnect() {
    if (!isConnected) {
        return true;
    }

    capture.release();
    isConnected = false;
    std::cout << "USB cam " << name << " disconnected" << std::endl;

    return true;
}


bool USBCamera::captureFrame() {
    if (!isConnected || !capture.isOpened()) {
        return false;
    }

    return capture.read(currFrame);
}


bool USBCamera::isAvailable() const {
    return capture.isOpened();
}


void USBCamera::setDeviceIndex(int index) {
    deviceIndex = index;
}