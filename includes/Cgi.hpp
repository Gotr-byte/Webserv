#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>

class Cgi
{
public:
	Cgi(std::string type, char **env, int socket_fd, const char *args);
	~Cgi();

	std::string get_file_name();
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
	Cgi();
	Cgi &operator=(const Cgi &other);
	Cgi(const Cgi &other);

	std::string _type;
	// int _tubes[2];
	int _cgi_pid;
	int _file_fd;
	char **_env;
	char *_args;
	std::string _file_name;
};