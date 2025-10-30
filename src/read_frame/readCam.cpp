#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Camera {
private:
    string source;
    string windowName;
    VideoCapture cap;
    bool running;

public:
    Camera(const string& src, const string& winName)
        : source(src), windowName(winName), running(false) {}

    bool open() {
        if (isdigit(source[0]) && source.size == 1)
            cap.open(stoi(source));     // usb cam
        else
            cap.open(source);           // IP cam URL
        
        if (!cap.isOpened()) {
            cerr << "Error: Can't open camera source: " << source << endl;
            return false;
        }
        return true;
    }

    void process() {
        running = true;
        Mat frame, gray, filtered;

        while (running) {
            if (!cap.read(frame) || frame.empty()) {
                cerr << "Warning: No frame captured from: " << source << endl;
                break;
            }

            // convert to grayscale
            cvtColor(frame, gray, COLOR_BGR2GRAY);

            // Gaussian blur
            GaussianBlur(gray, filtered, Size(5, 5), 1.5);

            // Sobel edge detection
            Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
            Sobel(filtered, grad_x, CV_165, 1, 0, 3);
            Sobel(filtered, grad_y, CV_165, 0, 1, 3);
            convertScaleAbs(grad_x, abs_grad_x);
            convertScaleAbs(grad_y, abs_grad_y);
            addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, filtered);

            imshow(windowName, filtered);

            // exit when press ESC
            if (waitKey(1) == 27) {
                stop();
                break;
            }
        }
        cap.release();
        destroyWindow(windowName);
    }
};


int main() {
    string ipCamURL = "rtsp://your.IP.CAM.address";
    string usbCamIdx = "0";

    Camera ipCam(ipCamURL, "IP Cam");
    Camera usbCam(usbCamIdx, "USB Cam");

    if (!ipCam.open() || !usbCam.open()) {
        cerr << "Failed to open cameras." << endl;
        return -1;
    }

    thread t1(&Camera::process, &ipCam);
    thread t2(&Camera::process, &usbCam);

    t1.join();
    t2.join();

    return 0;
}
