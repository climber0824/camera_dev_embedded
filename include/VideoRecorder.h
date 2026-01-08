#ifndef VIDEO_RECORDER_H
#define VIDEO_RECORDER_H

#include <string>
#include <opencv2/opencv.hpp>


class VideoRecorder {
private:
    cv::VideoWriter writer;
    std::string outputPath;
    bool isRecording;
    int codec;
    double fps;
    cv::Size frameSize;

public:
    VideoRecorder();
    ~VideoRecorder();

    bool startRecording(const std::string &filename, int fps, 
                        const cv::Size &frameSize,
                        int codec = cv::VideoWriter::fourcc('X', 'V', 'I', 'D'));
    bool stopRecording();
    bool writeFrame(const cv::Mat &frame);

    bool getRecordingStatus() const;
    std::string getOutputPath() const;
};

#endif