#include <iostream>

#include "IPCamera.h"


IPCamera::IPCamera(const std::string &id, const std::string &name,
                   const std::string &ip, int port)
    : Camera(id, name), ipAddress(ip), port(port) {}


IPCamera::~IPCamera() {
    disconnect();
}


std::string IPCamera::buildStreamUrl() {
    if (!streamUrl.empty()) {
        return streamUrl;
    }

    // default RTSP URL format
    std::string url = "rtsp://";
    if (!username.empty() && !password.empty()) {
        url += username + ":" + password + "@";
    }
    url += ipAddress + ":" + std::to_string(port) + "/stream";

    return url;
}


bool IPCamera::connect() {
    if (isConnected) {
        return true;
    }

    std::string url = buildStreamUrl();
    capture.open(url);

    if (!capture.isOpened()) {
        std::cerr << "Failed to open IP cam: " << url << std::endl;
        return false;
    }

    capture.set(cv::CAP_PROP_FRAME_WIDTH, width);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    isConnected = true;
    std::cout << "IP cam " << name << " connected" << std::endl;

    return true;
}


bool IPCamera::disconnect() {
    if (!isConnected) {
        return true;
    }

    capture.release();
    isConnected = false;
    std::cout << "IP cam " << name << " disconnected" << std::endl;

    return true;
}


bool IPCamera::captureFrame() {
    if (!isConnected || !capture.isOpened()) {
        return false;
    }

    return capture.read(currFrame);
}


bool IPCamera::isAvailable() const {
    return capture.isOpened();
}


void IPCamera::setCredentials(const std::string &user, const std::string &pass) {
    username = user;
    password = pass;
}


void IPCamera::setStreamUrl(const std::string &url) {
    streamUrl = url;
}


std::string IPCamera::getIPAddress() const {
    return ipAddress;
}