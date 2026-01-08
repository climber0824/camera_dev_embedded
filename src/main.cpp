#include "SurveillanceSystem.h"
#include "USBCamera.h"
#include "IPCamera.h"

int main() {
    SurveillanceSystem system;

    // Add cameras
    auto cam1 = std::make_shared<USBCamera>("test1", "USB cam", 0);
    // auto cam2 = std::make_shared<IPCamera>("back", "Backyard", "192.168.1.100");
    // cam2->setCredentials("admin", "pass123");

    system.addCamera(cam1);
    // system.addCamera(cam2);

    // Configure motion detection
    system.enableMotionDetection("front", 30);  // threshold=30
    // system.enableMotionDetection("back", 25);   // threshold=25

    // Start recording
    system.startRecording("front", "front_door.avi");
    // system.startRecording("back", "backyard.avi");

    // Start surveillance
    system.start();

    // Display live view (press 'q' to quit)
    system.displayAllCameras();

    // Stop system
    system.stop();

    return 0;
}
