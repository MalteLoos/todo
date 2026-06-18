#include "backend.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    int port = argc > 1 ? std::stoi(argv[1]) : 8080;

    // Claude: start the recieve loop here:
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) { std::cerr << "Failed to create socket\n"; return 1; }

    int yes = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverFd, (sockaddr*)&addr, sizeof(addr)) < 0) { std::cerr << "Bind failed\n"; return 1; }
    if (listen(serverFd, 10) < 0) { std::cerr << "Listen failed\n"; return 1; }

    std::cout << "Listening on port " << port << "\n";

    Backend backend;
    backend.acceptLoop(serverFd);
}
