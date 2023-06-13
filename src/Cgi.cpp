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
	//create a file
	const char* filename = "../HTML/cgi-bin/outfile.txt";
    int outfile = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (outfile == -1) {
        std::cerr << "Error opening the file.\n";
        throw(CgiException());
    }
	if (_cgi_pid == 0)
	{
		//read file
		char buffer[100];
		ssize_t bytesRead;

		while ((bytesRead = read(outfile, buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytesRead] = '\0';  // Null-terminate the buffer
			std::cout << buffer;
		}

		//set standard output to file
		dup2(outfile, STDOUT_FILENO);
		close(outfile);

		//create enviroment
		char* scriptPath = (char*)args;
        char* _args[] = { "/usr/bin/python3", scriptPath, NULL };
        char* _env[] = {
            "REQUEST_METHOD=GET",
            "SCRIPT_NAME=/cgi-bin/create_file.py",
            "SERVER_PROTOCOL=HTTP/1.1",
            "SERVER_SOFTWARE=MyWebServer/1.0",
            NULL
        };
		execve(_args[0], _args, _env);
		throw(CgiException());
		//close fds
		close(outfile);
	}
}
