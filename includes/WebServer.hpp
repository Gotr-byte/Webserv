#pragma once

#include <sys/types.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <poll.h>
#include <cstddef>
#include <limits>
#include <algorithm>
#include <fstream>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string>
#include <stdio.h>
#include <map>
#include <cctype>
#include <set>
#include <string.h>
#include <unistd.h>
#include <sstream>

#include "Client.hpp"
#include "Socket.hpp"
#include "Cgi.hpp"
#include "SocketConfig.hpp"
#include "ConfigCheck.hpp"
#include "Response.hpp"
// #include <cstring.h>
// #include <string.h>

#define BUF_SIZE 1024
#define POLL_TIMEOUT 200

class WebServer
{
	public:
		WebServer(std::string path, char **env);
		~WebServer();

		int			setupListeningSockets();
		void		loopPollEvents();
		void		conductPolling();
		void		acceptClients(int server_fd);
		void		performGet(int client_fd);
		void		performUpload(int client_fd);
		void		performDelete(int client_fd);
		void		performCgi(int client_fd);
		void		sendResponse(int client_fd);
		void		killClient(std::vector<struct pollfd>::iterator it);
		bool		validateFilename(std::string filename);
		std::string convertIPv4ToString(const struct in_addr& address);
		void		place_in_file(std::string line_to_file);
		void		generate_cgi_querry(std::map<std::string, std::string> &new_request);
		std::string toHex(int value);

	private:
		WebServer();
		WebServer(const WebServer &other);
		WebServer& operator = (const WebServer & other);

		char **_env;
		int							listening_port_no;
		std::vector<struct pollfd>  poll_fds;
		std::map<int, Client>       fds_clients;
		std::vector<int>            listening_socket_fds;
		std::map<int, SocketConfig> configs;
		std::string                 config_path;

		class				InvalidFileDownloadException : public std::exception
		{
			public:
				virtual const char* what() const throw()
				{
					return("Invalid location exception\n");
				}
		};
};