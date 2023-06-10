#include "../includes/Cgi.hpp"

Cgi::Cgi(std::string type, int request_id) : _type(type)
{
	std::stringstream string_stream;
	string_stream << request_id;
	_file_name = type + string_stream.str() + ".tmp";
}
Cgi::~Cgi() {}

std::string Cgi::get_file_name()
{
	return (_file_name);
}

// Use try whilie running
void Cgi::run(char **env, const char *args)
{
	this->_cgi_pid = fork();
	if (_cgi_pid < 0)
	{
		std::cerr << "Error with fork\n";
		throw(CgiException());
	}
	if (_cgi_pid == 0)
	{
		std::string python_path = "/usr/bin/python3";
		_args[0] = (char *)python_path.c_str();
		_args[1] = (char *)args;
		_args[2] = NULL;
		_env = env;
		execve(_args[0], _args, _env);
		throw(CgiException());
	}
}