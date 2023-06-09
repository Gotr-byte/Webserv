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

#define BUF_SIZE 1024
#define POLL_TIMEOUT 200

class WebServer
{
	public:
		WebServer(std::string path);
		~WebServer();

		void		setupListeningSockets();
		void		loopPollEvents();

	private:

		void		conductPolling();
		void		acceptClients(int server_fd);
		void		performCgi(int client_fd);
		void		performGet(int client_fd);
		void		performDelete(int client_fd);
		void		performUpload(int client_fd);
		void		sendResponse(int client_fd);
		void		killClient(std::vector<struct pollfd>::iterator it);

		bool		deleteIfExists(std::string filename);
		bool		validateFilename(std::string filename);

		std::string convertIPv4ToString(const struct in_addr& address);
		std::string toHex(int value);

		std::vector<struct pollfd>	poll_fds;
		std::vector<int>            listening_socket_fds;
		std::map<int, Client>       fds_clients;
		std::map<int, SocketConfig> configs;
		int							listening_port_no;
		std::string                 config_path;
};
