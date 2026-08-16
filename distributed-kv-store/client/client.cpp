#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include "../config.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: client \"GET key\" | \"PUT key value\" | \"DELETE key\" [port]\n";
        return 1;
    }

    int port = SERVER1_PORT;
    if (argc > 2) {
        port = std::stoi(argv[2]);
    }

    std::string command = argv[1];
    if (command.empty() || command.back() != '\n') {
        command.push_back('\n');
    }

    const int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "socket() failed\n";
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port));
    inet_pton(AF_INET, SERVER1_IP, &addr.sin_addr);

    if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "connect() failed\n";
        close(sock);
        return 1;
    }

    send(sock, command.c_str(), command.size(), 0);

    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    const ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        std::cout << std::string(buffer, static_cast<size_t>(bytes));
    }

    close(sock);
    return 0;
}
