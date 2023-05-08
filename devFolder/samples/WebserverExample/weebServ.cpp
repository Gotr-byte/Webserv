#include <iostream>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

const int MAX_EVENTS = 10;

int main() {
    // Create the server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Failed to create server socket\n";
        return 1;
    }

    // Set up the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8000);

    // Bind the server socket to the address
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Failed to bind server socket\n";
        return 1;
    }

    // Set the server socket to listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        std::cerr << "Failed to listen on server socket\n";
        return 1;
    }

    // Create a pollfd struct for the server socket
    struct pollfd server_pollfd;
    server_pollfd.fd = server_socket;
    server_pollfd.events = POLLIN;

    // Create a vector to hold all the client sockets
    std::vector<int> client_sockets;

    while (true) {
        // Create an array of pollfd structs for all the sockets
        std::vector<pollfd> pollfds(1 + client_sockets.size());
        pollfds[0] = server_pollfd;
        for (size_t i = 0; i < client_sockets.size(); i++) {
            pollfds[i+1].fd = client_sockets[i];
            pollfds[i+1].events = POLLIN;
        }

        // Wait for events on all the sockets
        if (poll(pollfds.data(), pollfds.size(), -1) < 0) {
            std::cerr << "Failed to poll sockets\n";
            return 1;
        }

        // Handle events on the server socket
        if (pollfds[0].revents & POLLIN) {
            // Accept a new connection and add it to the list of client sockets
            struct sockaddr_in client_address;
            socklen_t client_address_size = sizeof(client_address);
            int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_size);
            if (client_socket < 0) {
                std::cerr << "Failed to accept new connection\n";
            } else {
                std::cout << "Accepted new connection from " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << "\n";
                client_sockets.push_back(client_socket);
            }
        }

        // Handle events on the client sockets
        for (size_t i = 0; i < client_sockets.size(); i++) {
            if (pollfds[i+1].revents & POLLIN) {
                // Receive data from the client socket
                char buffer[1024];
                ssize_t num_bytes = recv(client_sockets[i], buffer, sizeof(buffer), 0);
                if (num_bytes < 0) {
                    std::cerr << "Failed to receive data from client socket\n";
                } else if (num_bytes == 0) {
                    // The client socket has closed, remove it from the list of client sockets
                    std::cout << "Client " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << " disconnected\n";
                    close(client_sockets[i]);
                    client_sockets.erase(client_sockets.begin() + i);
                    i--;
                } else {
                    // Echo the data back to the client
                    if (send(client_sockets[i], buffer, num_bytes, 0) < 0) {
                        std::cerr << "Failed to send data to client socket\n";
                    }
                }
            }
        }
    }

    // Close all the client sockets
    for (int client_socket : client_sockets) {
        close(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
