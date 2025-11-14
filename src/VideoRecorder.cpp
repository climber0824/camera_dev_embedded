#include <iostream>

#include "VideoRecorder.h"


VideoRecorder::VideoRecorder()
    : isRecording(false), codec(cv::VideoWriter::fourcc('X', 'V', 'I', 'D')),
      fps(30.0), frameSize(640, 480) {}
      

VideoRecorder::~VideoRecorder() {
    stopRecording();
}


bool VideoRecorder::startRecording(const std::string &filename, int fps,
                                   const cv::Size &framesize, int codec) {
    if (isRecording) {
        std::cerr << "Already recording. Stop current recording first." << std::endl;
        return false;
    }

    this->outputPath = filename;
    this->fps = fps;
    this->frameSize = frameSize;
    this->codec = codec;

    // open video writer
    writer.open(filename, codec, fps, frameSize, true);

    if (!writer.isOpened()) {
        std::cerr << "Failed to open video writer: " << filename << std::endl;
        return false;
    }

    isRecording = true;
    std::cout << "Started recording to: " << filename << std::endl;

    return true;
}


bool VideoRecorder::stopRecording() {
    if (!isRecording) {
        return true;
    }

    writer.release();
    isRecording = false;
    std::cout << "Stopped recording: " << outputPath << std::endl;

    return true;
}


bool VideoRecorder::writeFrame(const cv::Mat &frame) {
    if (!isRecording || !writer.isOpened() || frame.empty()) {
        return false;
    }

    // resize frame is necessary
    cv::Mat frameToWrite = frame;
    if (frame.size() != frameSize) {
        cv::resize(frame, frameToWrite, frameSize);
    }

    // convert to 3-channel if needed
    if (frameToWrite.channels() == 1) {
        cv::cvtColor(frameToWrite, frameToWrite, cv::COLOR_GRAY2BGR);
    }
    else if(frameToWrite.channels() == 4) {
        cv::cvtColor(frameToWrite, frameToWrite, cv::COLOR_BGRA2BGR);
    }

    writer.write(frameToWrite);

    return true;
}


bool VideoRecorder::getRecordingStatus() const {
    return isRecording;
}


std::string VideoRecorder::getOutputPath() const {
    return outputPath;
}