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
#include <ctime>
#include <stdio.h>
#include <map>
#include <set>
#include <deque>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include "ServerConfig.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "../includes/Request.hpp"
#include "Colors.hpp"

#define MAX_CLIENTS 3
#define LISTENING_SOCKET

class HTTP_server
{
public:
    HTTP_server(std::string path, char **env);
    ~HTTP_server();

    void server_conducts_poll();
    void	InitFdsClients();
    void server_port_listening(int i);
    size_t  findHeaderLength(int fd);
    std::map<std::string, std::string> mapping_request_header(int i);
    int running();
    void create_listening_sock(int port);
    void create_pollfd_struct();
    void ProcessUpload(std::vector<Request>::iterator req);
    void server_loop();
    void deleteContent(std::vector<Request>::iterator req);
    bool CheckForClientTimeout(int i);
    std::set<int> activeClientIdx;
    std::string read_file(const std::string &filename);
    void tokenizing(std::map<std::string, std::string> & request, std::string line_to_tokenize);
    void place_in_file(std::string line_to_file);
    std::string toHex(int value);
    void removeWhitespaces(std::string &string);
    void send_response(int i, std::vector<Request>::iterator req);

private:
    HTTP_server();
    HTTP_server(const HTTP_server &other);
    HTTP_server& operator = (const HTTP_server & other);

    char **_env;
    std::string _path;
    int nfds;
    int res;
    std::vector<std::pair<int, Client> >    FdsClients;
    std::vector<int>   fd_index;
    int currently_served_quantity;
    struct sockaddr_in client_addr;
    Client *clients;
    time_t currentTime;
    time_t timeoutDuration;
    std::deque<int> pending_connections;
    struct pollfd *fds;
    int timeout;
    Request tmp;
    std::vector<char *> HTTP_requests;
    socklen_t client_len;
    std::string http_response;
    std::string filename;
    std::string content;
    std::string message;
    char *line;
    std::deque<std::string> lines;
    std::string HTTP_line;
    std::string content_type;
    std::vector<int> listening_socket_fd;
    int sentBytes[20];
    std::vector<ServerConfig> ConfigVec;
    int listening_port_no;
    class				InvalidFileDownloadException : public std::exception
		{
			public:
				virtual const char* what() const throw()
				{
					return("Invalid location exception\n");
				}
		};
};