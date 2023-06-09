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
#include "RequestProcessor.hpp"
#include "../includes/Request.hpp"

#define MAX_CLIENTS 3
#define LISTENING_SOCKET

class HTTP_server
{
public:
    HTTP_server();
    ~HTTP_server();
    void server_conducts_poll();
    void	InitFdClientVec();
    // void perform_get_request(int i);
    void server_port_listening(int i);
    std::map<std::string, std::string> server_mapping_request(int i);
    int handle_request(std::string path);
    void create_listening_sock(int port);
    void create_pollfd_struct();
    void server_loop();
    bool CheckForTimeout(int i);
    std::set<int> activeClientIdx;
    std::string read_file(const std::string &filename);
    void tokenizing(std::map<std::string, std::string> & request, std::string line_to_tokenize);
    void place_in_file(std::string line_to_file);
    std::string toHex(int value);
    void    removeWhitespaces(std::string &string);
    // void get_static_html(int i);
    void get_request(int i, std::vector<Request>::iterator req);
    // void get_error_site(int i, std::string error_page);

private:
    HTTP_server(const HTTP_server &other);
    HTTP_server& operator = (const HTTP_server & other);
    int nfds;
    int res;
    std::vector<std::pair<int, Client> >    FdClientVec;
    std::vector<int>   fd_index;
    int currently_served_quantity;
    struct sockaddr_in client_addr;
    Client *clients;
    std::time_t currentTime;
    std::time_t timeoutDuration;
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
    // std::map< int, std::map<std::string, std::string> > request;
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