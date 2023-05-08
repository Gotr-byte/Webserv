#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "server.h"

void launch(struct Server* server)
{
    char buffer[30000];
    char* hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 144\n\n<!DOCTYPE html><html><head><title>Prototype page</title></head><body><h1>Prototype page header </h1><p>Prototype page content.</p></body></html>";
    int addrlen = sizeof(server->address);
    int new_socket = accept(server->socket,  (struct sockaddr*)&server->address, (socklen_t*)&addrlen);
    while(1)
    {
        printf("========WAITING FOR CONNECTION==========\n");
        read(new_socket, buffer, 30000);
        printf("buffer: %s\n", buffer);
        write(new_socket, hello, strlen(hello));
    }
    close(new_socket);
}

int main()
{
    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 69, 10, launch);
    server.launch(&server);
}