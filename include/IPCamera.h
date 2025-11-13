#ifndef IP_CAMERA_H
#define IP_CAMERA_H

#include "Camera.h"


class IPCamera : public Camera {
private:
    std::string ipAddress;
    int port;
    std::string username;
    std::string password;
    std::string streamUrl;
    cv::VideoCapture capture;
    
    std::string buildStreamUrl();

public:
    IPCamera(const std::string &id, const std::string &name, 
             const std::string &ip, int port = 554);
    ~IPCamera() override;

    bool connect() override;
    bool disconnect() override;
    bool captureFrame() override;
    bool isAvailable() const override;

    void setCredentials(const std::string &user, const std::string &pass);
    void setStreamUrl(const std::string &url);
    std::string getIPAddress() const;
};

#endif