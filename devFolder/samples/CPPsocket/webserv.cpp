#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "webserv.hpp"

void    serverLoop(const int server_fd)
{
    int new_socket, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nGoodbye World!";
    char    buf[1024];

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
      perror("accept failed");
      exit(EXIT_FAILURE);
    }


    valread = read(new_socket, buf, 1024);
    std::cout << buf << std::endl;


    write(new_socket, hello, strlen(hello));
    close(new_socket);
}

int main(int argc, char *argv[]) {
    int server_fd;
    const char *port = "8080";

    server_fd = openPortSocket(port);
    std::cout << "Server started on: " << port << std::endl;

    while (1)
    {
        serverLoop(server_fd);
    }
    return 0;
}