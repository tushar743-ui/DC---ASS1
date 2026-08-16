#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../config.h"

int get_node_port(const std::string& key) {
    (void)key;
    return PRIMARY_PORT;
}

std::string send_command(const std::string& command, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return "ERROR: socket creation failed";

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);
    inet_pton(AF_INET, PRIMARY_HOST, &server_addr.sin_addr);

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        return "ERROR: could not connect to server";
    }

    std::string msg = command + "\n";
    send(sock, msg.c_str(), msg.size(), 0);

    char buf[BUFFER_SIZE] = {};
    recv(sock, buf, sizeof(buf) - 1, 0);

    close(sock);

    std::string response(buf);
    while (!response.empty() && (response.back() == '\n' || response.back() == '\r'))
        response.pop_back();

    return response;
}

int main() {
    std::cout << "=== Distributed KV Store Client ===\n";
    std::cout << "Commands: PUT <key> <value> | GET <key> | DELETE <key> | quit\n\n";

    std::string line;
    while (true) {
        std::cout << "kv> ";
        if (!std::getline(std::cin, line)) break;
        if (line == "quit" || line == "exit") break;
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string op, key;
        ss >> op >> key;

        if (key.empty()) {
            std::cout << "Usage: PUT <key> <value> | GET <key> | DELETE <key>\n";
            continue;
        }

        int port     = get_node_port(key);
        std::string response = send_command(line, port);

        std::cout << "Response: " << response << "\n";
    }

    std::cout << "Bye!\n";
    return 0;
}