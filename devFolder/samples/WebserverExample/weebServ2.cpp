#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>

int main() {
    // Create the server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Could not create server socket\n";
        return -1;
    }

    // Bind the socket to a specific IP address and port
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(12345);
    if (bind(server_socket, (sockaddr*) &address, sizeof(address)) == -1) {
        std::cerr << "Could not bind socket to port\n";
        close(server_socket);
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_socket, SOMAXCONN) == -1) {
        std::cerr << "Could not listen on socket\n";
        close(server_socket);
        return -1;
    }

    std::cout << "Server running on port 12345\n";

    // Vector to hold client sockets
    std::vector<int> client_sockets;

    // Loop to accept incoming connections
    while (true) {
        sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int client_socket = accept(server_socket, (sockaddr*) &client_address, &client_address_size);
        if (client_socket == -1) {
            std::cerr << "Could not accept client connection\n";
            continue;
        }

        // Add client socket to vector
        client_sockets.push_back(client_socket);

        std::cout << "Client " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << " connected\n";

        // Receive data from client
        char buffer[4096];
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            std::cerr << "Could not receive data from client\n";
            close(client_socket);
            continue;
        }

        std::cout << "Received " << bytes_received << " bytes from client: " << buffer << "\n";

        // Send response back to client
        std::string response = "Hello from server!";
        if (send(client_socket, response.c_str(), response.size(), 0) == -1) {
            std::cerr << "Could not send response to client\n";
            close(client_socket);
            continue;
        }

        std::cout << "Sent response to client\n";
    }

    // Close all client sockets
    for (int client_socket : client_sockets) {
        close(client_socket);
    }

    // Close server socket
    close(server_socket);

    return 0;
}