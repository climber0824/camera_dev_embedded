#ifndef USB_CAMERA_H
#define USB_CAMERA_H

#include "Camera.h"


class USBCamera : public Camera {
private:
    int deviceIndex;
    cv::VideoCapture capture;

public:
    USBCamera(const std::string &id, const std::string &name, int deviceIndex = 0);
    ~USBCamera() override;

    bool connect() override;
    bool disconnect() override;
    bool captureFrame() override;
    bool isAvailable() const override;

    void setDeviceIndex(int index);
};

#endif
