#pragma once

#if defined(__APPLE__)
#include <sys/types.h>
#endif
#if defined(__linux__)
#include <unistd.h>
#include <string.h>
#include <string>
#include <sstream>
#endif

#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <iostream>

class Socket
{
    public:
		Socket(int port, std::string ip);
        ~Socket();

        int     server_fd;

    private:
		struct addrinfo hints;
		struct addrinfo *server_info;
};