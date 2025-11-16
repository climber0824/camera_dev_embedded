#include <iostream>
#include <thread>
#include <chrono>

#include "SurveillanceSystem.h"
#include "USBCamera.h"
#include "IPCamera.h"


int main() {
    std::cout << "=== Surveillance System Demo ===" << std::endl;

    SurveillanceSystem system;

    // add USB camera
    auto usbCam = std::make_shared<USBCamera>("usb_cam_1", "demo", 0);
    system.addCamera(usbCam);

    // add IP camera
    auto ipCam = std::make_shared<IPCamera>("ip_cam_1", "demo", "ip_address", 554);
    ipCam->setCredentials("admin", "your_password");
    system.addCamera(ipCam);

    // enable motion detection
    system.enableMotionDetection("usb_cam_1", 30);
    system.enableMotionDetection("ip_cam_1", 25);

    // start recording
    system.startRecording("usb_cam_1", "usb_demo.avi");
    system.startRecording("ip_cam_1", "ip_demo.avi");

    // start surveillance
    if (system.start()) {
        std::cout << "Surveillance system started" << std::endl;

        // run for 60 seconds
        std::this_thread::sleep_for(std::chrono::seconds(60));

        // stop system
        system.stop();
        std::cout << "Surveillance system stopped" << std::endl;
    }

    return 0;
}