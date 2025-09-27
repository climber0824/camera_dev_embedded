#include "stdio.h"
#include "stdlib.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

void read_wifi_cam() {
    string url = "IPCam address";
    cv::VideoCapture cap(url);

    if (!cap.isOpened()) {
        std::cerr << "Error: can't open IP camera stream\n";
        return;
    }

    cv::Mat frame;

    while (true) {
        if (!cap.read(frame)) {
            std::cerr << "Error: fail to grab frame\n";
            break;
        }

        cv::imshow("IP cam", frame);

        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
}


int main() {

    read_wifi_cam();
    return 0;
}

