#pragma once

#if defined(__APPLE__)
#include <sys/types.h>
#endif
#if defined(__linux__)
#include <unistd.h>
#include <string.h>
#endif

#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstdlib>
#include <stdio.h>
#include <string>

#define MAX_CLIENTS 3

class Socket{
    public:
        Socket();
        ~Socket();
		Socket(int port, std::string ip);
        int server_fd;
		int opt;
		struct sockaddr_in server_addr;
    private:
        Socket(const Socket &other);
        Socket &operator = (const Socket &other);
};