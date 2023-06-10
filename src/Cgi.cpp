#include "../includes/Cgi.hpp"

Cgi::Cgi(std::string type, char **env, int request_id, const char *args): 
_type(type), _env(env){

	std::stringstream string_stream;
	string_stream << request_id;
	_file_name = type + string_stream.str() + ".tmp";

	_args = (char*)args;
}
Cgi::~Cgi(){}

std::string Cgi::get_file_name(){
	return (_file_name);
}

//Use try whilie running
void Cgi::run(){
	// Open a file for writing
  	// std::ofstream outputFile(_file_name);

	this->_cgi_pid = fork();
	if (_cgi_pid < 0){
		throw(CgiException());
	}
	if(_cgi_pid == 0){
		execve(&(_args[0]), &(_args), _env);
		throw(CgiException());         
	}
}