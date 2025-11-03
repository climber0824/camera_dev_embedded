#include "LocalServer.h"

LocalServer::LocalServer() : server_fd(-1), running(false) {}


bool LocalServer::start(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_id < 0) {
        perror("socket");
        return false;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_int addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return false;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        return false;
    }

    running = true;
    std::thread(&LocalSer::acceptLoop, this).detach();
    std::cout << "{LocalServer} listening on port" << port << std::endl;

    return true;
}


void LocalServer::acceptLoop() {
    while (running) {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(server_id, (sockaddr*)&client_addr, &len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        std::thread(&LocalServer::handleClient, this, client_fd).detach();
    }
}


void LocalServer::handleClient(int client_fd) {
    char header[128];
    ssize_t n = read(client_fd, header, sizeof(header));
    if (n <= 0) {
        close(client_fd);
        return;
    }

    std::string headerStr(header, n);
    size_t sep = headerStr.find('|');
    if (sep == std::string::npos) {
        std::cerr << "{LocalServer} invalid header\n";
        close(client_fd);
        return;
    }

    std::string timestamp = headerStr.substr(0, sep);
    size_t imgSize = std::stoul(headerStr.substr(sep + 1));

    std::cout << "{LocalServer} receiving image: " << imgSize
                << " bytes at" << timestamp << std::endl;

    std::vector<uchar> imgBuf(imgSize);
    size_t received = 0;

    while (received < imgSize) {
        ssize_t bytes = read(client_fd, imgBuf.data() + received,
                            imgSize - received);
        if (bytes <= 0) break;
        received += bytes;
    }

    if (received == imgSize) {
        cv::Mat img = cv::imdecode(imgBuf, cv::IMREAD_COLOR);
        if (!img.empty()) {
            std::string filename = "/images/" + timestamp + ".jpg";
            for (auto &c : filename) {
                if (c == ' ' || c == ':') c = '_';                
            }
            cv::imwrite(filename, img);
            std::cout << "{LocalServer} saved image: " << filename << std::endl;
        }
    }
    else {
        std::cerr << "{LocalServer} incomplete image received.\n";
    }

    close(client_fd);
}


void LocalServer::stop() {
    running = false;
    if (server_fd >= 0)
        close(server_id);
}
