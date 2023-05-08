#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFSIZE 1024

void send_file(int sockfd, char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Error opening file");
        return;
    }
    
    char buffer[BUFSIZE];
    int n;
    while ((n = fread(buffer, 1, BUFSIZE, fp)) > 0) {
        if (send(sockfd, buffer, n, 0) != n) {
            perror("Error sending file");
            break;
        }
    }
    
    fclose(fp);
}

int main(int argc, char const *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFSIZE] = {0};
    char *filename = "example.txt";
    char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    printf("New connection accepted.\n");

    // Receive GET request
    if (recv(new_socket, buffer, BUFSIZE, 0) < 0) {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }

    printf("Received message: %s\n", buffer);

    // Send response header
    if (send(new_socket, response, strlen(response), 0) < 0) {
        perror("send failed");
        exit(EXIT_FAILURE);
    }

    // Send file contents
    send_file(new_socket, filename);

    printf("File sent.\n");

    // Close connection
    close(new_socket);
    close(server_fd);

    return 0;
}


