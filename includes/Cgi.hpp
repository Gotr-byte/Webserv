#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "Colors.hpp"
#include <string.h>

class Cgi
{
public:
	Cgi(std::string type, int request_id);
	~Cgi();

	std::string get_file_name();
	void run(char **env, const char *args);

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
	int _cgi_pid;
	int _file_fd;
	std::string _file_name;
};