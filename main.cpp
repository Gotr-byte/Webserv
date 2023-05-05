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
#include <fcntl.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUF_SIZE 4096

//TODO perform the operations required by the documentation on NGINX, and note down the behaviour, also compare later
//TODO needs request parsing
//TODO needs response generating
//TODO error response
//TODO response
//TODO needs HTML, just a page with a file to download
//TODO check if the sockets need to be non blocking
//TODO send a simple error code response
//TODO check if the server has a asynchronous I/O
//TODO handle routing in the webserver

// server {
//     port: 1896;
//     host: localhost;
//     server_name: test_server;
//     error_page: ./www/errors/;
//     limit_body_size: 100;
//     allowed_methods: GET;
//     location / {
//         root: ./www/html/;
//         index: index.html;
//         redirect: https://google.de/;
//         allowed_methods: POST, GET;
//         autoindex: on;
//     }
// }

// Yes, in order for the operations on the server to be non-blocking, the sockets need to be set up in a specific way.

// First, the server socket should be set to non-blocking mode by using the fcntl() function to set the O_NONBLOCK flag on the socket file descriptor. This allows the server to accept incoming connections without blocking, even if there are no pending connections.

// Next, when a new client connection is accepted, the socket for that connection should also be set to non-blocking mode using the same method. This allows the server to read and write to the socket without blocking, even if the client is not ready to send or receive data.

// Finally, the server should use a non-blocking I/O model, such as the epoll() or select() system calls, to handle multiple client connections without blocking. These system calls allow the server to monitor multiple sockets for I/O activity and respond to them asynchronously, without waiting for any particular socket to complete its I/O operation.

// In summary, setting the sockets to non-blocking mode and using a non-blocking I/O model are the key steps to making the server operations non-blocking. This allows the server to handle multiple connections simultaneously and efficiently, without blocking on any particular connection.

//Handle various pages
// New client connected
// Received message from client: GET /favicon.ico HTTP/1.1
// Host: localhost:8088
// Connection: keep-alive
// sec-ch-ua: "Google Chrome";v="111", "Not(A:Brand";v="8", "Chromium";v="111"
// sec-ch-ua-mobile: ?0
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/111.0.0.0 Safari/537.36
// sec-ch-ua-platform: "macOS"
// Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8
// Sec-Fetch-Site: same-origin
// Sec-Fetch-Mode: no-cors
// Sec-Fetch-Dest: image
// Referer: http://localhost:8088/index.html
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en-US,en;q=0.9



// Sure thing, fellow adventurer! When we set a socket to non-blocking, it means that when we call functions like recv or send, they will return immediately, even if there is no data available to receive or the socket buffer is full and cannot send any more data. This is different from the default blocking mode, where these functions will wait until there is data available to receive or the buffer has space to send data.

// To set a socket to non-blocking mode, we can use the fcntl function with the F_SETFL command and the O_NONBLOCK flag. Here's an example code snippet:

// c++
// Copy code
// int flags = fcntl(sock_fd, F_GETFL, 0);
// flags |= O_NONBLOCK;
// fcntl(sock_fd, F_SETFL, flags);
// In this example, sock_fd is the socket file descriptor that we want to set to non-blocking mode. We first use fcntl with the F_GETFL command to get the current socket flags. We then bitwise OR the O_NONBLOCK flag with the current flags to set the non-blocking mode, and finally use fcntl with the F_SETFL command to set the new flags.

// By setting the socket to non-blocking mode, we can then use the poll function to wait for events on the socket without blocking the main thread of our program. This allows us to handle multiple connections at the same time, which is useful for server applications.

// I hope that helps! Let me know if you have any more questions.

// int create_socket(int etc_port,  struct sockaddr_in *server_addr)
// {
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd < 0) {
//         perror("Error creating server socket");
//         exit(EXIT_FAILURE);
//     }

//     //set the socket to non blocking
//     fcntl(server_fd, F_SETFL, O_NONBLOCK);

//     // Set the server socket options
//     int opt = 1;
//     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
//         perror("Error setting server socket options");
//         exit(EXIT_FAILURE);
//     }

//     // Bind the server socket to a specific port
//     // struct sockaddr_in server_addr;
//     // memset(&server_addr, 0, sizeof(server_addr));
//     // server_addr.sin_family = AF_INET;
//     // server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr->sin_port = htons(etc_port);
//     if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(*server_addr)) < 0) {
//         perror("Error binding server socket etc");
//         exit(EXIT_FAILURE);
//     }

//     // Listen for incoming connections on the server socket
//     if (listen(server_fd, MAX_CLIENTS) < 0) {
//         perror("Error listening on server socket");
//         exit(EXIT_FAILURE);
//     }

//     return(server_fd);
// }


int handleRequest(int port, int etc_port) {

    // Create a socket for the server
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    //set the socket to non blocking
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

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
        perror("Error binding server socket port");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections on the server socket
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Error listening on server socket");
        exit(EXIT_FAILURE);
    }

    // int etc_fd = create_socket(etc_port, &server_addr);

   int etc_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (etc_fd < 0) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    //set the socket to non blocking
    fcntl(etc_fd, F_SETFL, O_NONBLOCK);

    // Set the server socket options
    // int opt = 1;
    if (setsockopt(etc_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Error setting server socket options");
        exit(EXIT_FAILURE);
    }

    // Bind the server socket to a specific port
    struct sockaddr_in server_addr_etc;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr_etc.sin_family = AF_INET;
    server_addr_etc.sin_addr.s_addr = INADDR_ANY;
    server_addr_etc.sin_port = htons(etc_port);
    if (bind(etc_fd, (struct sockaddr *)&server_addr_etc, sizeof(server_addr_etc)) < 0) {
        perror("Error binding server socket etc");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections on the server socket
    if (listen(etc_fd, MAX_CLIENTS) < 0) {
        perror("Error listening on server socket");
        exit(EXIT_FAILURE);
    }

    // Initialize the pollfd struct for the server socket
    struct pollfd fds[MAX_CLIENTS + 1];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    fds[1].fd = etc_fd;
    fds[1].events = POLLIN;

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

            std::cout << "New client connected on server fd\n";
        }
        if (fds[1].revents & POLLIN) {

            // Accept a new connection from a client
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(etc_fd, (struct sockaddr *)&client_addr, &client_len);
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

            std::cout << "New client connected on etc fd\n";
        }

        // Check for events on any of the connected client sockets
        for (unsigned long i = 1; i < clients.size(); i++) 
        {
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
                //here routing needs to happen
                // Send a response message back to the client
                if(buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T')
                {
                  std::cout << "Received message from client: " << buf << std::endl;
                  std::string message = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 14\n\nGoodbye World!";
                  n = send(clients[i], message.c_str(), message.length(), 0);
                  // n = send(clients[i], message.c_str(), 5, 0);
                  if (n < 0) {
                      perror("Error sending data to client");
                      exit(EXIT_FAILURE);
                  }
                }
            }
        }
    }

    // Close all connected client sockets
    for (unsigned long i = 0; i < clients.size(); i++) {
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
  handleRequest(conf.listen, 4000);
}