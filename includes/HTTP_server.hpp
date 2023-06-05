#pragma once

#if defined(__APPLE__)
#include <sys/types.h>
#endif
#if defined(__linux__)
#include <unistd.h>
#endif

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
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
#include "ServerConfig.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "RequestProcessor.hpp"

#define MAX_CLIENTS 300
#define LISTENING_SOCKET

class HTTP_server
{
public:
    HTTP_server();
    ~HTTP_server();
    void server_conducts_poll();
    void perform_get_request(int i);
    void server_port_listening(int i);
    void server_mapping_request(int i);
    int handle_request(std::string path);
    void create_listening_sock(int port);
    void create_pollfd_struct();
    void server_loop();
    std::string read_file(const std::string &filename);
    void tokenizing(std::map<std::string, std::string> &request, std::string line_to_tokenize);
    // void                            print_map();
    void place_in_file(std::string line_to_file);
    std::string toHex(int value);
    void    removeWhitespaces(std::string &string);

private:
    HTTP_server(const HTTP_server &other);
    HTTP_server& operator = (const HTTP_server & other);
    int nfds;
    int res;
    // int server_fd; // move to socket
    // int opt; // move to socket
    // struct sockaddr_in server_addr; // move to socket

    struct sockaddr_in client_addr;
    std::vector<Client> clients;
    // Client clients[MAX_CLIENTS - 3];
    struct pollfd fds[MAX_CLIENTS];
    std::vector<char *> HTTP_requests;
    socklen_t client_len;
    std::string http_response;
    std::string filename;
    std::string content;
    std::string message;
    std::map< int, std::map<std::string, std::string> > request;
    char *line;
    std::deque<std::string> lines;
    std::string HTTP_line;
    std::string content_type;
    std::vector<int> listening_socket_fd;
    int sentBytes[20];
    std::vector<ServerConfig> ConfigVec;
    int listening_port_no;
};