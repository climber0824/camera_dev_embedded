#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>

#define WIDTH 640
#define HEIGHT 480
#define CHANNELS 3
#define FRAME_SIZE (WIDTH * HEIGHT * CHANNELS)


// shared memory layout: [frame1][frame2]
struct SharedMemory {
    unsigned char frame1[FRAME_SIZE];   // wifi cam
    unsigned char frame2[FRAME_SIZE];   // USB cam
};

struct CameraArgs {
    std::string name;
    std::string url;    // can be RTSP/HTTP or device index
};

typedef SharedMemory SharedMemory;
typedef CameraArgs   CameraArgs;


void* read_cam(void* args) {
    CameraArgs* cam = (CameraArgs*)args;

    cv::VideoCapture cap;

    // if url is numeric string, treat as device index
    if (isdigit(cam->url[0])) {
        cap.open(std::stoi(cam->url));
    }
    else {
        cap.open(cam->url);
    }

    if (!cap.isOpened()) {
        std::cerr << "Can't open" << cam->name << std::endl;
        return NULL;
    }

    cv::Mat frame;
    while (true) {
        if (!cap.read(frame)) {
            std::cerr << "Failed to grab frame from " << cam->name << std::endl;
            break;
        }

        cv::imshow(cam->name, frame);

        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    cap.release();
    return NULL;
}


void read_cam_in_thread(CameraArgs* wifi_cam, CameraArgs* usb_cam) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, read_cam, wifi_cam);
    pthread_create(&t2, NULL, read_cam, usb_cam);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}



int main() {
    CameraArgs wifiCam = {"Wifi Cam", "rtsp::user:pass@ip_address/stream1"};
    CameraArgs usbCam = {"USB Cam", "0"};

    read_cam_in_thread(&wifiCam, &usbCam);

    cv::destroyAllWindows();
    return 0;    
}




