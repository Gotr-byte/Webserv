#pragma once

#include "WebServer.hpp"
#include "Client.hpp"
#include "Response.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <map>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include <signal.h>

#define WRITE_END 1
#define READ_END 0

class Cgi
{
	public:
		Cgi(Client & c);
		~Cgi();

		void run();

		class CgiException : public std::exception
		{
			public:
				virtual const char *what() const throw()
				{
					return ("exception: CGI\n");
				}
		};


	private:
		static void handleTimeout(int signum);
		bool is_python3_installed();

		Client & 					client;
		std::vector<std::string>	enviromentals;
};