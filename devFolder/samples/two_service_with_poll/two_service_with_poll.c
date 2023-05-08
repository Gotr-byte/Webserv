#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>

#define MAX_EVENTS 2
#define HTTP_PORT 8080
#define UDP_PORT 8888
#define MAX_FILENAME_LEN 256
#define MAX_FILESIZE 1048576 // 1MB

// Function to handle HTTP requests
void handle_http_request(int sockfd)
{
    char filename[MAX_FILENAME_LEN] = "index.html";
    char response[MAX_FILESIZE];
    int n;

    // Open file
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Read file contents into response buffer
    n = fread(response, 1, MAX_FILESIZE, file);
    if (n == 0) {
        perror("Error reading file");
        fclose(file);
        return;
    }

    // Send HTTP response header
    char header[512];
    sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", n);
    send(sockfd, header, strlen(header), 0);

    // Send file contents
    send(sockfd, response, n, 0);

    // Close file
    fclose(file);
}

// Function to handle UDP packets
void handle_udp_packet(int sockfd)
{
    struct sockaddr_in addr;
    char buffer[1024];
    socklen_t len = sizeof(addr);
    int n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, &len);
    if (n < 0) {
        perror("Error receiving UDP packet");
        return;
    }

    // Print received message to console
    printf("Received UDP packet from %s:%d: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), buffer);
}

int main()
{
    int http_sockfd, udp_sockfd;
    struct sockaddr_in http_addr, udp_addr;

    // Create HTTP socket
    http_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (http_sockfd < 0) {
        perror("Error creating HTTP socket");
        exit(EXIT_FAILURE);
    }

    // Bind HTTP socket to port 8080
    memset(&http_addr, 0, sizeof(http_addr));
    http_addr.sin_family = AF_INET;
    http_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    http_addr.sin_port = htons(HTTP_PORT);
    if (bind(http_sockfd, (struct sockaddr *)&http_addr, sizeof(http_addr)) < 0) {
        perror("Error binding HTTP socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming HTTP requests
    if (listen(http_sockfd, 5) < 0) {
        perror("Error listening on HTTP socket");
        exit(EXIT_FAILURE);
    }

    // Create UDP socket
    udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sockfd < 0) {
        perror("Error creating UDP socket");
        exit(EXIT_FAILURE);
    }

    // Bind UDP socket to port 8888
    if (bind(udp_sock, (struct sockaddr *)&udp_server_address, sizeof(udp_server_address)) < 0) {
        perror("UDP bind failed");
        exit(EXIT_FAILURE);
    }
    printf("UDP socket bound to port 8888\n");

    // Start listening for connections
    listen_for_connections(tcp_sock, udp_sock);

    // Clean up sockets
    close(tcp_sock);
    close(udp_sock);

    return 0;
    }

void listen_for_connections(int tcp_sock, int udp_sock) {
    struct pollfd pollfds[2];
    pollfds[0].fd = tcp_sock;
    pollfds[0].events = POLLIN;
    pollfds[1].fd = udp_sock;
    pollfds[1].events = POLLIN;

    int timeout = 30 * 1000; /* 30 seconds in ms */

    printf("Starting to listen for socket events.\n");

    while (1) {
        int res = poll(pollfds, 2, timeout);
        if (res == -1) {
            /* An error has occurred */
            perror("poll() failed");
            exit(EXIT_FAILURE);
        } else if (res == 0) {
            /* Timeout has occurred */
            printf("Still waiting for socket events.\n");
        } else {
            /* Check which sockets had events occur */
            if (pollfds[0].revents & POLLIN) {
                /* TCP socket has a new connection */
                struct sockaddr_in client_address;
                socklen_t client_address_len = sizeof(client_address);
                int client_sock = accept(tcp_sock, (struct sockaddr *)&client_address, &client_address_len);
                if (client_sock < 0) {
                    perror("TCP accept failed");
                    exit(EXIT_FAILURE);
                }
                printf("TCP client connected from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                close(client_sock);
            }
            if (pollfds[1].revents & POLLIN) {
                /* UDP socket has a new message */
                struct sockaddr_in client_address;
                socklen_t client_address_len = sizeof(client_address);
                char buffer[BUF_SIZE];
                memset(buffer, 0, BUF_SIZE);
                ssize_t num_bytes = recvfrom(udp_sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&client_address, &client_address_len);
                if (num_bytes < 0) {
                    perror("UDP recvfrom failed");
                    exit(EXIT_FAILURE);
                }
                printf("UDP message received from %s:%d: %s\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer);
                close(udp_sock);
                // sendto(sock, response, strlen(response), 0, (struct sockaddr *) &udp_client_addr, sizeof(udp_client_addr));
                // handle_udp_message(udp_sock, buffer, &client_address, client_address_len);
            }
        }
    }
}
