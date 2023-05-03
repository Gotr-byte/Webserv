#include <iostream>
#include <fstream>
#include "./includes/Configuration.hpp"

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUF_SIZE 1024

int handleRequest(int port) {

    // Create a socket for the server
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    // Set the server socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Error setting server socket options");
        exit(EXIT_FAILURE);
    }

    // Bind the server socket to a specific port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding server socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections on the server socket
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Error listening on server socket");
        exit(EXIT_FAILURE);
    }

    // Initialize the pollfd struct for the server socket
    struct pollfd fds[MAX_CLIENTS + 1];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    // Initialize the list of connected client sockets
    std::vector<int> clients;

    // Start the main loop
    while (true) {

        // Wait for events on any of the sockets
        int nfds = clients.size() + 1;
        int res = poll(fds, nfds, -1);
        if (res < 0) {
            perror("Error polling sockets");
            exit(EXIT_FAILURE);
        }

        // Check for events on the server socket
        if (fds[0].revents & POLLIN) {

            // Accept a new connection from a client
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
            if (client_fd < 0) {
                perror("Error accepting client connection");
                exit(EXIT_FAILURE);
            }

            // Add the client socket to the list of connected clients
            clients.push_back(client_fd);

            // Initialize the pollfd struct for the client socket
            fds[clients.size()].fd = client_fd;
            fds[clients.size()].events = POLLIN;
            fds[clients.size()].revents = 0;

            std::cout << "New client connected\n";
        }

        // Check for events on any of the connected client sockets
        for (unsigned long i = 0; i < clients.size(); i++) {
            if (fds[i+1].revents & POLLIN) {

                // Receive data from the client
                char buf[BUF_SIZE];
                memset(buf, 0, BUF_SIZE);
                int n = recv(clients[i], buf, BUF_SIZE, 0);
                if (n < 0) {
                    perror("Error receiving data from client");
                    exit(EXIT_FAILURE);
                }

                // Print the received message
                std::cout << "Received message from client: " << buf << std::endl;

                // Send a response message back to the client
                std::string message = "Hello from server!";
                n = send(clients[i], message.c_str(), message.length(), 0);
                if (n < 0) {
                    perror("Error sending data to client");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    // Close all connected client sockets
    for (int i = 0; i < clients.size(); i++) {
        close(clients[i]);
    }

    // Close the server socket
    close(server_fd);

    return 0;
}

int main (int argc, char **argv)
{
  if (argc != 2)
  {
      std::cerr << "Error: invalid number of arguments, please enter one argument\n";
      return 1;
  }
  Configuration conf;
  if (conf.parseSetListen(argv[1], "listen"))
    return 2;
  std::cout << conf.listen << "\n";
  handleRequest(conf.listen);
}