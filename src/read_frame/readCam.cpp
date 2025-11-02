#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace cv;

class Camera {
private:
    string source;
    string windowName;
    VideoCapture cap;
    int time_sleep = 10;
    bool running;
    bool show_image = false;
    bool save_image = true;

    string getTimestampFilename(const string camera_type) {
        auto now = chrono::system_clock::now();
        time_t t = chrono::system_clock::to_time_t(now);
        tm local_tm = *localtime(&t);

        stringstream ss;
        ss << camera_type + "_images/"
            << put_time(&local_tm, "%Y%m%d_%H%M%S")
            << ".jpg";

        return ss.str();
    }

    Mat edgeDetection(Mat origin_frame) {
        Mat gray, filtered;

        // convert to grayscale
        cvtColor(origin_frame, gray, COLOR_BGR2GRAY);
        
        // Gaussian blur
        GaussianBlur(gray, filtered, Size(5, 5), 1.5);
        
        // Sobel edge detection
        Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
        Sobel(filtered, grad_x, CV_16F, 1, 0, 3);
        Sobel(filtered, grad_y, CV_16F, 0, 1, 3);
        convertScaleAbs(grad_x, abs_grad_x);
        convertScaleAbs(grad_y, abs_grad_y);
        addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, filtered);
    
        return filtered;
    }





public:
    Camera(const string& src, const string& winName)
        : source(src), windowName(winName), running(false) {}

    bool open() {
        if (isdigit(source[0]) && source.size() == 1)
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
        Mat frame;

        while (running) {
            cap >> frame;

            if (!cap.read(frame) || frame.empty()) {
                cerr << "Warning: No frame captured from: " << source << endl;
                break;
            }

            if (save_image) {
                string filename = getTimestampFilename("");
                if (windowName.find("USB") != string::npos)
                    filename = getTimestampFilename("USB");
                else if (windowName.find("IP") != string::npos)
                    filename = getTimestampFilename("IP");
                else
                    cerr << "Error: wrong camera type, only USB or IP" << endl;

                if (!imwrite(filename, frame)) {
                    cerr << "Error: failed to write image: " << filename << endl;
                }
                else {
                    cout << "Save: " << filename << endl;
                }
            }

            if (show_image) {
                imshow(windowName, filtered);

                // exit when press ESC
                if (waitKey(1) == 27) {
                    break;
                }
            }

            // sleep
            this_thread::sleep_for(chrono::seconds(time_sleep));
        }
        cap.release();
        
        if (show_image) {
            destroyWindow(windowName);
        }
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
