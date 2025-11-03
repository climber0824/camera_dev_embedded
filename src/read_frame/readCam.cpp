#include "readCam.h"
using namespace std;
using namespace cv;

Camera::Camera(const string& src, const string& winName)
    : source(src), windowName(winName), running(false) {}

string Camera::getTimestampFilename(const string camera_type) {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&t);

    stringstream ss;
    ss << camera_type + "_images/"
       << put_time(&local_tm, "%Y%m%d_%H%M%S")
       << ".jpg";

    return ss.str();
}

Mat Camera::edgeDetection(const Mat& origin_frame) {
    Mat gray, filtered;

    cvtColor(origin_frame, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, filtered, Size(5, 5), 1.5);

    Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
    Sobel(filtered, grad_x, CV_16F, 1, 0, 3);
    Sobel(filtered, grad_y, CV_16F, 0, 1, 3);
    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, filtered);

    return filtered;
}

bool Camera::detectMotion(const Mat& prev, const Mat& curr, float motion_thresh) {
    if (prev.empty() || curr.empty())
        return false;

    Mat diff, gray, thresh;
    absdiff(prev, curr, diff);
    cvtColor(diff, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, gray, Size(5, 5), 0);
    threshold(gray, thresh, 25, 255, THRESH_BINARY);

    float motionPercent = (countNonZero(thresh) * 100.0f) / (thresh.rows * thresh.cols);
    printf("motion percent: %f\n", motionPercent);
    return motionPercent > motion_thresh;
}

bool Camera::open() {
    if (isdigit(source[0]) && source.size() == 1)
        cap.open(stoi(source));  // USB cam
    else
        cap.open(source);        // IP cam URL

    if (!cap.isOpened()) {
        cerr << "Error: Can't open camera source: " << source << endl;
        return false;
    }
    return true;
}

void Camera::process() {
    running = true;
    Mat frame, prevFrame;

    while (running) {
        if (!cap.read(frame) || frame.empty()) {
            cerr << "Warning: No frame captured from: " << source << endl;
            break;
        }

        bool has_motion = detectMotion(prevFrame, frame, motion_thresh);
        if (has_motion) {
            cout << "[" << windowName << "] motion detected!" << endl;
        }

        frame.copyTo(prevFrame);

        if (save_image) {
            string filename;
            if (windowName.find("USB") != string::npos)
                filename = getTimestampFilename("USB");
            else if (windowName.find("IP") != string::npos)
                filename = getTimestampFilename("IP");
            else {
                cerr << "Error: wrong camera type, only USB or IP" << endl;
                continue;
            }

            if (!imwrite(filename, frame)) {
                cerr << "Error: failed to write image: " << filename << endl;
            } else {
                cout << "Save: " << filename << endl;
            }
        }

        if (show_image) {
            imshow(windowName, frame);
            if (waitKey(1) == 27) break;
        }

        this_thread::sleep_for(chrono::seconds(time_sleep));
    }

    cap.release();
    if (show_image)
        destroyWindow(windowName);
}

