#pragma once
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
#include <fstream>
#include <string>
#include <stdio.h>

#define MAX_CLIENTS 10

class RequestHandler
{
    public:
        int                 handleRequest(int port);
    private:
        int                 server_fd;
        int                 etc_fd;
        int                 opt;
        struct sockaddr_in  server_addr;
        struct sockaddr_in  server_addr_etc;
        struct sockaddr_in  client_addr;
        std::vector<int>    clients;
        struct pollfd       fds[MAX_CLIENTS];
        socklen_t           client_len;
        std::string         http_response;
        std::string         filename;
        std::string         content;
        std::string         message;
};