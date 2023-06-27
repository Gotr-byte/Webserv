#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

//whenever we add to the vector we need to add an element
//when we finish we set the elements fd to -1


const int MAX_CLIENTS = 9999;
const int BUFFER_SIZE = 1024;

std::string generateHttpResponse() {
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: 12\r\n";
    response += "\r\n";
    response += "Hello, world!";
    return response;
}

// void addClientFdToPollFds(int clientSocket, std::vector<pollfd> pollFds){
//     pollFds.push_back(clientSocket);
// }

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress{}, clientAddress{};
    socklen_t clientAddressLength;

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Set up server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    // Bind the socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return 1;
    }

    // Initialize pollfd structs
    // std::vector<pollfd> pollFds(MAX_CLIENTS + 1);
    std::vector<pollfd> pollFds(1);

    pollFds[0].fd = serverSocket;
    pollFds[0].events = POLLIN;

  
    // std::vector<pollfd>::iterator it;
    // for (it = pollFds.begin() + 1; it != pollFds.end(); ++it) {
    //     it->fd = -1;
    // }

    // Main server loop
    while (true) {
        // Call poll and wait for events
        if (poll(&pollFds[0], pollFds.size(), -1) == -1) {
            std::cerr << "Error in poll" << std::endl;
            return 1;
        }

        // Check for events on each descriptor
        for (size_t i = 0; i < pollFds.size(); ++i) {
            // Handle new client connections
            if (i == 0 && pollFds[i].revents & POLLIN) {
                // Accept the new connection
                clientAddressLength = sizeof(clientAddress);
                if ((clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientAddressLength)) ==
                    -1) {
                    std::cerr << "Failed to accept connection" << std::endl;
                    continue;
                }

                // Find an empty slot in the pollFds vector
                size_t clientIndex = 1;
                for (; clientIndex < pollFds.size(); ++clientIndex) {
                    if (pollFds[clientIndex].fd == -1) {
                        pollFds[clientIndex].fd = clientSocket;
                        pollFds[clientIndex].events = POLLIN;
                        break;
                    }
                }

                //Create client struct
                struct pollfd clientPoll;
                clientPoll.fd = clientSocket;
                clientPoll.events = POLLIN;


                //addClientFdToPollFds(clientSocket, pollFds);
                pollFds.push_back(clientPoll);

                // If the array is full, close the client socket
                // if (clientIndex == pollFds.size() - 1) {
                //     std::cerr << "Maximum number of clients reached. Closing the connection." << std::endl;
                //     close(clientSocket);
                // }
            }
            // Handle data from existing clients
            else if (pollFds[i].revents & POLLIN) {
                char buffer[BUFFER_SIZE];
                memset(buffer, 0, sizeof(buffer));

                // Receive data from the client
                ssize_t bytesRead = recv(pollFds[i].fd, buffer, sizeof(buffer), 0);
                if (bytesRead <= 0) {
                    // Connection closed or error occurred, close the client socket
                    close(pollFds[i].fd);
                    pollFds[i] = pollFds.back();
                    pollFds.pop_back();
                } else {
                    // Process the received data
                    std::cout << "Received data from client: " << buffer << std::endl;

                    // Check if it's a GET request
                    if (strncmp(buffer, "GET", 3) == 0) {
                        // Generate the HTTP response
                        std::string httpResponse = generateHttpResponse();

                        // Send the response back to the client
                        send(pollFds[i].fd, httpResponse.c_str(), httpResponse.length(), 0);
                        // pollFds[i].fd = -1;
                    }
                }
            }
        }
    }

    return 0;
}
