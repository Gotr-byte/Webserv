#pragma once

# if defined (__APPLE__)
#  include <sys/types.h>
# endif
# if defined (__linux__)
#  include <unistd.h>
# endif

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <poll.h>


#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <map>
#include <deque>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sstream>

#define MAX_CLIENTS 10


class HTTP_server
{
    public:
		void							server_conducts_poll();
		void							perform_get_request(int i);
		void							server_port_listening(int client_fd, int i);
		void							server_mapping_request(int i);
        int                             handleRequest(int port);
        void                            create_listening_sock(int port);
        void                            create_pollfd_struct();
        void                            server_loop();
        std::string                     read_file(const std::string& filename);
        void                            tokenizing( std::map<std::string, std::string>& request, std::string line_to_tokenize);
        void                            print_map();
        void                            place_in_file(std::string line_to_file);
        // void                            place_in_file(char* line_to_file);
    private:
        int                             nfds;
        int                             res;
        int                             server_fd;
        int                             opt;
        struct sockaddr_in              server_addr;
        struct sockaddr_in              client_addr;
        std::vector<int>                clients;
        std::vector<char*>              HTTP_requests;
        struct pollfd                   fds[MAX_CLIENTS];
        socklen_t                       client_len;
        std::string                     http_response;
        std::string                     filename;
        std::string                     content;
        std::string                     message;
        std::map<std::string, std::string>    request;
        char*                           line;
        std::deque<std::string>         lines;
        std::string                     HTTP_line;
        std::string content_type;
        std::vector<int>                listening_socket_fd;
        // char                            HTTP_body[1024];
};