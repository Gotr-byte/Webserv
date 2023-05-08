#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFSIZE 1024

void handle_root(int connfd) {
    char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Welcome to the root page</h1>";
    write(connfd, response, strlen(response));
}

void handle_page1(int connfd) {
    char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Welcome to page 1</h1>";
    write(connfd, response, strlen(response));
}

void handle_page2(int connfd) {
    char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Welcome to page 2</h1>";
    write(connfd, response, strlen(response));
}

void handle_not_found(int connfd) {
    char* response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
    write(connfd, response, strlen(response));
}

int main(int argc, char const *argv[]) {
    int sockfd, connfd, addrlen;
    struct sockaddr_in address;
    char buffer[BUFSIZE] = {0};

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified address and port
    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d\n", PORT);

    while (1) {
        addrlen = sizeof(address);
        connfd = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        if (connfd < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Read the HTTP request
        read(connfd, buffer, BUFSIZE);
        printf("buffer %s", buffer);
        // Find the requested path
        char path[BUFSIZE] = {0};
        sscanf(buffer, "GET %s", path);

        // Handle the request based on the path
        if (strcmp(path, "/") == 0) {
            handle_root(connfd);
        } else if (strcmp(path, "/page1") == 0) {
            handle_page1(connfd);
        } else if (strcmp(path, "/page2") == 0) {
            handle_page2(connfd);
        } else {
            handle_not_found(connfd);
        }

        close(connfd);
    }

    return 0;
}