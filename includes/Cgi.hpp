#pragma once


#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "WebServer.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "Client.hpp"
#include "Colors.hpp"
#include "Response.hpp"
#include <string.h>
#include <map>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include <signal.h>

class Cgi
{
public:
	Cgi(Client & c);
	~Cgi();

	std::string get_file_name();
	void run();
	void print_request(std::map<std::string, std::string> my_map);
	bool is_python3_installed();
	bool is_python_file(const std::string &str);
	bool is_query_string(std::vector<Response>::iterator it_req);
	void print_enviromentals();
	std::string create_request_body_file(std::vector<Response>::iterator it_req);
	void smart_sleep(long set_miliseconds);

	class CgiException : public std::exception
	{
	public:
		virtual const char *what() const throw()
		{
			return ("exception: CGI\n");
		}
	};

	Client & client;

private:
	Cgi();
	Cgi &operator=(const Cgi &other);
	Cgi(const Cgi &other);

	std::string id;
	std::vector<std::string> enviromentals;
	std::string _type;
	int _cgi_pid;
	int _file_fd;
	std::string _file_name;
};