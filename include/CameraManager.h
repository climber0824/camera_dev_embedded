#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include <vector>
#include <map>
#include <memory>
#include <mutex>

#include "Camera.h"


class CameraManager {
private:
    std::map<std::string, std::shared_ptr<Camera>> cameras;
    std::mutex cameraMutex;

public:
    CameraManager();
    ~CameraManager();

    bool addCamera(std::shared_ptr<Camera> camera);
    bool removeCamera(const std::string &id);
    std::shared_ptr<Camera> getCamera(const std::string &id);
    std::vector<std::shared_ptr<Camera>> getAllCameras();

    bool connectAll();
    bool disconnectAll();

    int getCameraCount() const;
    std::vector<std::string> getCameraIds() const;
};

#endif