// #pragma once
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <arpa/inet.h>

#define SERVER_PORT 8000
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd, max_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set server socket options
    int optval = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind server socket to port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Set server socket to listen mode
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Set server socket to non-blocking mode
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    // Create poll file descriptor
    struct pollfd fds[MAX_CLIENTS + 1];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    max_fd = 0;

    // Main loop
    while (true) {
        int num_fds = poll(fds, max_fd + 1, -1);
        if (num_fds < 0) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        // Check for new client connections
        if (fds[0].revents & POLLIN) {
            if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // Set client socket to non-blocking mode
            flags = fcntl(client_fd, F_GETFL, 0);
            fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

            // Add client socket to poll file descriptor
            int i;
            for (i = 1; i <= MAX_CLIENTS; i++) {
                if (fds[i].fd == 0) {
                    fds[i].fd = client_fd;
                    fds[i].events = POLLIN;
                    break;
                }
            }
            if (i > MAX_CLIENTS) {
                std::cerr << "Too many clients" << std::endl;
                close(client_fd);
            }

            if (i > max_fd) {
                max_fd = i;
            }

            std::cout << "New client connected" << std::endl;
        }

    // Handle client

        for (int i = 1; i <= max_fd; i++) {
            if (fds[i].fd == 0) {
                continue;
            }

            if (fds[i].revents & POLLIN) {
                int num_bytes = read(fds[i].fd, buffer, sizeof(buffer));
                if (num_bytes < 0) {
                    perror("read");
                    close(fds[i].fd);
                    fds[i].fd = 0;
                }
                else if (num_bytes == 0) {
                    std::cout << "Client disconnected" << std::endl;
                    close(fds[i].fd);
                    fds[i].fd = 0;
                }
                else {
                    std::cout << "Received " << num_bytes << " bytes from client" << std::endl;

                    // Write incoming data to file
                    FILE *fp = fopen("received_file", "wb");
                    fwrite(buffer, 1, num_bytes, fp);
                    fclose(fp);
                }
            }
        }
    }
    return 0;
}