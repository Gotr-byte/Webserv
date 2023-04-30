#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int isRequestValid(char *input)
{
    // char *validGetRequest = "GET /index.html HTTP/1.1\nHost: example.com";
    char *validGetRequest = "GET /index.html HTTP/1.1\n";
    int i = 0;

    if (strlen(input) != strlen(validGetRequest))
        return(2);
    while (input[i])
    {
        if (input[i] != validGetRequest[i])
            return(1);
        i++;
    }
    return(0);
}

int main() {
    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Set socket options
    int optval = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        std::cerr << "Error setting socket options\n";
        return 1;
    }

    // Bind the socket to a port
    sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) < 0) {
        std::cerr << "Error binding socket to port\n";
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        std::cerr << "Error listening on socket\n";
        return 1;
    }

    // Accept incoming connections and process requests
    while (true) {
        sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int client_socket = accept(server_socket, reinterpret_cast<sockaddr*>(&client_address), &client_address_size);
        if (client_socket < 0) {
            std::cerr << "Error accepting incoming connection\n";
            continue;
        }

        // Read request data from client
        char buffer[1024];
        int read_bytes = read(client_socket, buffer, sizeof(buffer) - 1);
        if (read_bytes < 0) {
            std::cerr << "Error reading from client socket\n";
            close(client_socket);
            continue;
        }

        // Null-terminate the buffer
        buffer[read_bytes] = '\0';
        // if(!isRequestValid(buffer))
        std::cout << buffer << std::endl;
        // else
        //     std::cout << "invalid HTTP request\n";


        // Print the request to console

        // Send a response to the client
        std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello, world!";
        int write_bytes = write(client_socket, response.c_str(), response.size());
        if (write_bytes < 0) {
            std::cerr << "Error writing to client socket\n";
            close(client_socket);
            continue;
        }

        // Close the client socket
        close(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
