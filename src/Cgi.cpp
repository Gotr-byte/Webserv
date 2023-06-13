#include "../includes/Cgi.hpp"
#include <stdio.h>

//TODO catch the moment when the file is uploaded
//TODO have a timestamp to prevent the CGI to block the server
void Cgi::print_request(std::map<std::string, std::string> my_map)
{
    std::map<std::string, std::string>::iterator it;
    switch (color_index) {
        case 0:
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                std::cout << RED << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
            }
            color_index++;
            break;
        case 1:
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                std::cout << YELLOW << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
            }
            color_index++;
            break;
        case 2:
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                std::cout << CYAN << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
            }
            color_index++;
            break;
        case 3:
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                std::cout << CYAN << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
            }
            color_index = 0;
            break;
        default:
            std::cout << "Invalid choice." << std::endl;
            break;
    }
}

Cgi::Cgi(std::string type, int request_id) : _type(type)
{
	std::stringstream string_stream;
	string_stream << request_id;
	_file_name = type + string_stream.str() + ".tmp";
	color_index = 0;
}
Cgi::~Cgi() {}

std::string Cgi::get_file_name()
{
	return (_file_name);
}

// Use try whilie running
void Cgi::run(std::map<std::string, std::string> request)
{
	// print_request(request);

	std::string env_variable;
	
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
		//set standard output to file
		dup2(outfile, STDOUT_FILENO);
		close(outfile);

		//create arguments for execve
		std::string path_adder = "../HTML" + request["location:"];
		char* script_path = (char*)(path_adder).c_str();
        char* _args[] = { "/usr/bin/python3", script_path, NULL };
		//TODO change this to const
		//create enviroment
		int variables_number = 16;
		char *_env[variables_number + 1];

		//TEST
		_env[0] = "CONTENT_TYPE=text/html";  // Example value for Content-Type
		_env[1] = "GATEWAY_INTERFACE=CGI/1.1";
		_env[2] = "PATH_TRANSLATED=/path/to/file";
		_env[3] = "REQUEST_METHOD=POST";
		_env[4] = "CONTENT_LENGTH=1024";
		_env[5] = "SERVER_SOFTWARE=Weebserver";
		_env[6] = "SERVER_NAME=Weebserver.com";  // Example value for server name
		_env[7] = "HTTP_ACCEPT=text/html";  // Example value for Accept header
		_env[8] = "HTTP_ACCEPT_LANGUAGE=en-US";  // Example value for Accept-Language header
		_env[9] = "HTTP_USER_AGENT=Mozilla/5.0";  // Example value for User-Agent header
		_env[10] = "SCRIPT_NAME=../HTML/cgi-bin/print_params.py";  // Example value for script name
		_env[11] = "HTTP_REFERER=http://example.com"; 
		_env[12] = "PATH_TRANSLATED=../HTML/cgi-bin/print_params.py";
		_env[13] = "QUERY_STRING=param1=value3&param2=value4";
		_env[14] = "REQUEST_METHOD=POST";
		_env[15] = "CONTENT_LENGTH=512";
		_env[16] = "SERVER_SOFTWARE=MyWeebserver";
		_env[17] = NULL;
		//END OF TEST



		// env_variable = "REQUEST_METHOD=" + request["method:"];
		// _env[0] = (char*)env_variable.c_str();

		// env_variable = "SCRIPT_NAME=" + request["location:"];
		// _env[1] = (char*)env_variable.c_str();

		// env_variable = "SERVER_PROTOCOL=" + request["HTTP_version:"];
		// _env[2] = (char*)env_variable.c_str();

		// env_variable = "SERVER_SOFTWARE=Weebserver";
		// _env[3] = (char*)env_variable.c_str();

		// _env[4] = NULL;

		execve(_args[0], _args, _env);
		throw(CgiException());
	}


	//close fds
	close(outfile);
}



		// _env[0] = "CONTENT_TYPE=text/html";  // Example value for Content-Type
		// _env[1] = "GATEWAY_INTERFACE=CGI/1.1";
		// _env[2] = "PATH_TRANSLATED=/path/to/file";
		// _env[3] = "QUERY_STRING=param1=value1&param2=value2";
		// _env[4] = "REQUEST_METHOD=POST";
		// _env[5] = "CONTENT_LENGTH=1024";
		// _env[6] = "SERVER_SOFTWARE=Weebserver";
		// _env[7] = "SERVER_NAME=Weebserver.com";  // Example value for server name
		// _env[8] = "HTTP_ACCEPT=text/html";  // Example value for Accept header
		// _env[9] = "HTTP_ACCEPT_LANGUAGE=en-US";  // Example value for Accept-Language header
		// _env[10] = "HTTP_USER_AGENT=Mozilla/5.0";  // Example value for User-Agent header
		// _env[11] = "SCRIPT_NAME=../HTML/cgi-bin/print_params.py";  // Example value for script name
		// _env[12] = "HTTP_REFERER=http://example.com"; 
		// _env[13] = "PATH_TRANSLATED=../HTML/cgi-bin/print_params.py";
		// _env[14] = "QUERY_STRING=param1=value1&param2=value2";
		// _env[15] = "REQUEST_METHOD=POST";
		// _env[16] = "CONTENT_LENGTH=512";
		// _env[17] = "SERVER_SOFTWARE=MyWeebserver";
		// _env[18] = NULL;
