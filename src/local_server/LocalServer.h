#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <arpa/inet.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

class LocalServer {
private:
    int server_id;
    bool running;

    void acceptLoop();
    void handleClient(int client_fd);

public:
    LocalServer();
    bool start(int port = 5000);
    void stop();
};
