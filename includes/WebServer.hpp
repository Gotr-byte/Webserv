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
#include <algorithm>
#include <ctime>
#include <stdio.h>
#include <map>
#include <cctype>
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
#include "../includes/Response.hpp"
#include "Colors.hpp"
// #include <cstring.h>
// #include <string.h>

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
		std::map<int, ServerConfig> configs;
		std::string                 config_path;
		struct sockaddr_in 			client_addr;
		void cgi_file_upload(std::vector<Response>::iterator req);

		class				InvalidFileDownloadException : public std::exception
		{
			public:
				virtual const char* what() const throw()
				{
					return("Invalid location exception\n");
				}
		};
		// class				GenericServerException : public std::exception
		// {
		//     public:
		//         virtual const char* what() const throw()
		//         {
		//             return("Error when determining header lenghth\n");
		//         }
		// };
};