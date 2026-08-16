#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../config.h"
#include "storage.h"

Storage storage;

bool is_primary = false;

void replicate_to_replica(const std::string& command) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;

    sockaddr_in replica_addr{};
    replica_addr.sin_family      = AF_INET;
    replica_addr.sin_port        = htons(REPLICA_PORT);
    inet_pton(AF_INET, REPLICA_HOST, &replica_addr.sin_addr);

    if (connect(sock, (sockaddr*)&replica_addr, sizeof(replica_addr)) < 0) {
        std::cerr << "[Primary] Could not reach replica — skipping replication\n";
        close(sock);
        return;
    }

    send(sock, command.c_str(), command.size(), 0);

    char buf[BUFFER_SIZE] = {};
    recv(sock, buf, sizeof(buf), 0);
    close(sock);
}

std::string handle_command(const std::string& command) {
    std::istringstream ss(command);
    std::string op, key, value;
    ss >> op >> key;

    if (op == "PUT") {
        ss >> value;
        std::string result = storage.put(key, value);
        if (is_primary) {
            replicate_to_replica(command);
        }
        return result;

    } else if (op == "GET") {
        return storage.get(key);

    } else if (op == "DELETE") {
        std::string result = storage.remove(key);
        if (is_primary) {
            replicate_to_replica(command);
        }
        return result;
    }

    return "ERROR unknown command";
}

void* client_thread(void* arg) {
    int client_fd = *(int*)arg;
    delete (int*)arg;

    char buf[BUFFER_SIZE];

    while (true) {
        memset(buf, 0, sizeof(buf));
        int bytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (bytes <= 0) break;

        std::string command(buf);
        while (!command.empty() && (command.back() == '\n' || command.back() == '\r'))
            command.pop_back();

        std::cout << "[Server] Received: " << command << "\n";

        std::string response = handle_command(command) + "\n";
        send(client_fd, response.c_str(), response.size(), 0);
    }

    close(client_fd);
    return nullptr;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./server [primary|replica]\n";
        return 1;
    }

    is_primary = (std::string(argv[1]) == "primary");
    int port   = is_primary ? PRIMARY_PORT : REPLICA_PORT;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen"); return 1;
    }

    std::cout << "[" << argv[1] << "] Listening on port " << port << " ...\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t   client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) { perror("accept"); continue; }

        std::cout << "[Server] New connection accepted\n";

        int* fd_ptr = new int(client_fd);
        pthread_t tid;
        pthread_create(&tid, nullptr, client_thread, fd_ptr);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}