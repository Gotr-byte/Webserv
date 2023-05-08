#include <iostream>
#include <vector>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace std;

int main()
{
    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Set up the server address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) {
        perror("ERROR on listening");
        exit(1);
    }

    // Set up the pollfd structure
    struct pollfd pfd;
    pfd.fd = sockfd;
    pfd.events = POLLIN;

    while (true) {
        // Wait for an event on the socket
        int ret = poll(&pfd, 1, -1);
        if (ret < 0) {
            perror("ERROR on poll");
            exit(1);
        }
        else if (ret == 0) {
            continue;  // no events
        }

        // Accept the incoming connection
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        // Read data from the socket
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        int n = read(newsockfd, buffer, sizeof(buffer)-1);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }

        // Write data to the socket
        const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello, world";
        n = write(newsockfd, response, strlen(response));
        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }

        // Close the socket
        close(newsockfd);
    }

    // Close the listening socket
    close(sockfd);

    return 0;
}