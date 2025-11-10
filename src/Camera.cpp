#include "Camera.h"


Camera::Camera(const std::string &id, const std::string &name)
    : id(id), name(name), isConnected(false), width(640), height(480), fps(30) {}


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