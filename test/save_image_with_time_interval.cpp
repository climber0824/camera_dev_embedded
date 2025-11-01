#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace cv;
using namespace std;


string getTimestampFilename() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&t);

    stringstream ss;
    ss << "images/"
        << put_time(&local_tm, "%Y%m%d_%H%M%S")
        << ".jpg";
    
    return ss.str();
}


int main() {
    VideoCapture cap(0);
    int time_sleep = 10;

    if (!cap.isOpened()) {
        cerr << "Error: can't open camera" << endl;
        return -1;
    }

    cout << "Saving frames to /images/ every 10 seconds..." << endl;

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) {
            cerr << "Warning: empty frame, skipping..." << endl;
            continue;
        }

        string filename = getTimestampFilename();
        if (!imwrite(filename, frame)) {
            cerr << "Error: failed to write image: " << filename << endl;  
        }
        else {
            cout << "Saved: " << filename << endl;
        }

        // sleep
        this_thread::sleep_for(chrono::seconds(time_sleep));

    }

    cap.release();

    return 0;
}


