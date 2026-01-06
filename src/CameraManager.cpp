#include <iostream>

#include "CameraManager.h"


CameraManager::CameraManager() {}


CameraManager::~CameraManager() {
    disconnectAll();
}


bool CameraManager::addCamera(std::shared_ptr<Camera> camera) {
    std::lock_guard<std::mutex> lock(cameraMutex);

    if (!camera) {
        return false;
    }

    std::string id = camera->getId();
    if (cameras.find(id) != cameras.end()) {
        std::cerr << "Camera with ID: " << id << " already exists" << std::endl;
        return false; 
    }

    cameras[id] = camera;
    std::cout << "Camera " << id << " added to manager" << std::endl;

    return true;
}


bool CameraManager::removeCamera(const std::string &id) {
    std::lock_guard<std::mutex> lock(cameraMutex);

    auto it = cameras.find(id);
    if (it == cameras.end()) {
        return false;
    }

    it->second->disconnect();
    cameras.erase(it);
    std::cout << "Camera " << id << " removed from manager" << std::endl;

    return true;
}


std::shared_ptr<Camera> CameraManager::getCamera(const std::string &id) {
    std::lock_guard<std::mutex> lock(cameraMutex);

    auto it = cameras.find(id);
    if (it == cameras.end()) {
        return nullptr;
    }

    return it->second;
}


std::vector<std::shared_ptr<Camera>> CameraManager::getAllCameras() {
    std::lock_guard<std::mutex> lock(cameraMutex);

    std::vector<std::shared_ptr<Camera>> res;
    for (const auto &cam : cameras) {
        res.push_back(cam.second);
    }

    return res;
}


bool CameraManager::connectAll() {
    std::lock_guard<std::mutex> lock(cameraMutex);
    bool success = true;

    for (auto &cam : cameras) {
        if (!cam.second->connect()) {
            success = false;
            std::cerr << "Camera ID: " << cam.first << " not connected" << std::endl;
        }        
    }

    return success;
}


bool CameraManager::disconnectAll() {
    std::lock_guard<std::mutex> lock(cameraMutex);

    bool success = true;
    for (auto &cam : cameras) {
        if (!cam.second->disconnect()) {
            success = false;
            std::cerr << "Camera ID: " << cam.first << " not disconnected" << std::endl;
        }
    }

    return success;
}


int CameraManager::getCameraCount() const {
    return cameras.size();
}


std::vector<std::string> CameraManager::getCameraIds() const {
    std::vector<std::string> ids;

    for (const auto &cam : cameras) {
        ids.push_back(cam.first);
    }

    return ids;
}
