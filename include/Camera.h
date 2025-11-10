#ifndef CAMERA_H
#define CAMERA_H

#include <string.h>
#include <memory>
#include <opevcv2/opencv.hpp>


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
    viud setFPS(int fps);
};

#endif