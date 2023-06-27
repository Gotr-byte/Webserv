#include "../includes/Cgi.hpp"
#include <stdio.h>

//TODO send error
//TODO catch the moment when the file is uploaded
//TODO have a timestamp to prevent the CGI to block the server
//TODO check if works remoteley
//TODO test this if it does not work on a machine without python3
//TODO handle post request
//TODO get address and port form the server setup

// Global flag to track if timeout occurred
volatile sig_atomic_t timeoutOccurred = 0;

// Signal handler for timeout
void handleTimeout(int signum) {
    timeoutOccurred = 1;
}

bool Cgi::is_python3_installed() {
    const char* pythonPath = "/usr/bin/python3";
    if (access(pythonPath, X_OK) == 0) {
        return true;
    } else {
        return false;
    }
}

bool Cgi::is_python_file(const std::string& str) {
    if (str.length() < 3) {
        return false;
    }
	std::string target = ".py";
    std::string lastThree = str.substr(str.length() - 3);
    return (lastThree == target);
}

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

std::string	Cgi::create_request_body_file(std::vector<Request>::iterator it_req)
{
    char buf[BUF_SIZE];
	std::ostringstream	filename;

	filename << it_req->path.substr(0, it_req->path.rfind("/") + 1);
	filename << "request_body_" << it_req->id;
	std::ofstream outFile(filename.str(), std::ios::binary | std::ios::trunc);

	size_t	request_body_size = atol(it_req->requestHeader["Content-Length:"].c_str());
	size_t	remaining_bytes = request_body_size;

	while (remaining_bytes)
	{
		size_t chunk_bytes = std::min((size_t) BUF_SIZE, remaining_bytes);

    	memset(buf, 0, chunk_bytes);

    	int read_bytes;
    	read_bytes = recv(it_req->client_fd, buf, request_body_size, MSG_DONTWAIT);
		if (read_bytes <= 0)
			break;
		outFile.write(buf, read_bytes);
		remaining_bytes -= chunk_bytes;
	}
	outFile.close();
    memset(buf, 0, BUF_SIZE);
	std::cout << "cgi: filename is" << filename.str() << "\n";
    recv(it_req->client_fd, buf, BUF_SIZE, MSG_DONTWAIT);
	return (filename.str());
}

// Use try while running.
void Cgi::run(std::vector<Request>::iterator it_req)
{
	std::string env_variable;
	int infile;
	std::string body_path;

	const int timeoutDuration = 3;

	if(!is_python3_installed())
		throw(CgiException());
	if(!is_python_file(it_req->requestHeader["location:"]))
		throw(CgiException());
	int save_stdin = dup(STDIN_FILENO);
	int save_stdout = dup(STDOUT_FILENO);
	std::cout << "cgi: request header method [" << it_req->requestHeader["method:"] << "]\n";
	if (!is_query_string(it_req))
	{
		body_path = create_request_body_file(it_req);
		std::cout << "cgi: the body path is " << body_path << "\n";
		const char* in_filename = body_path.c_str();
		infile = open(in_filename, O_RDONLY);
		if (infile == -1) {
			std::cerr << "cgi: Error opening the input file.\n";
			throw(CgiException());
    	}
	}
	
	//create an output file
	const char* out_filename = "../HTML/cgi-bin/city_of_brass";
    int outfile = open(out_filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (outfile == -1) {
        std::cerr << "cgi: Error opening the outfile.\n";
        throw(CgiException());
    }

	this->_cgi_pid = fork();
	if (_cgi_pid < 0)
	{
		std::cerr << "Error with fork\n";
		throw(CgiException());
	}
	
	if (_cgi_pid == 0)
	{
		//set standard output to file
		dup2(outfile, STDOUT_FILENO);
		close(outfile);
		if(!is_query_string(it_req)){
			dup2(infile, STDIN_FILENO);
			close(infile);
		}

		//create arguments for execve
		std::string path_adder = "../HTML" + it_req->requestHeader["location:"];
		char* script_path = (char*)(path_adder).c_str();

		const char* path_to_python = "/usr/bin/python3";
    	char* _args[3];
		_args[0] = (char *)path_to_python;
		_args[1] = script_path;
		_args[2] = NULL;

		//create enviroment
		std::string env_variable;

		int i = 0;

		//represents the length of the request body in bytes, only if there is a body
		if(!is_query_string(it_req)){
			env_variable = "CONTENT_LENGTH=" + it_req->requestHeader["Content-Length:"];
			enviromentals.push_back(env_variable);
		}

		// CONTENT_TYPE: "text/html" represents the media type of the request body
		if(!is_query_string(it_req)){
			env_variable = "CONTENT_TYPE=" + it_req->requestHeader["Content-Type:"];
			enviromentals.push_back(env_variable);

		}

		// GATEWAY_INTERFACE: "CGI/1.1" (indicates the CGI interface version)
		env_variable = "GATEWAY_INTERFACE=CGI/1.1";
		enviromentals.push_back(env_variable);

		// PATH_INFO: "/path/to/resource" (represents the path to the requested resource)
		// env_variable = "PATH_INFO=../HTML/cgi-bin/ziggurat_magi_infile";
		if(!is_query_string(it_req)){
			env_variable = "PATH_INFO=" + body_path;
			enviromentals.push_back(env_variable);
		}

		// QUERY_STRING: "param1=value1&param2=value2" (represents the query string parameters)
		// in case of POST this should be read from file
		if (is_query_string(it_req)) {
			env_variable = "QUERY_STRING=" + it_req->requestHeader["query_string:"];
			enviromentals.push_back(env_variable);	
		}

		// REQUEST_METHOD: "POST" (indicates the HTTP request method)
		env_variable = "REQUEST_METHOD=" + it_req->requestHeader["method:"];
		// env_variable = "REQUEST_METHOD=POST";
    	enviromentals.push_back(env_variable);

		// REMOTE_ADDR: "192.168.0.1" (represents the IP address of the client)
		env_variable = "REMOTE_ADDR=127.0.0.1";
		// env_variable = "REMOTE_ADDR=" + it_req->;
    	enviromentals.push_back(env_variable);

		// SCRIPT_NAME: "/cgi-bin/script.cgi" (represents the path to the executed script)
		env_variable = "SCRIPT_NAME=" + it_req->requestHeader["location:"];
    	enviromentals.push_back(env_variable);

		// SERVER_NAME: "example.com" (represents the server's hostname or domain name)
		env_variable = "SERVER_NAME=" + it_req->config.getConfProps("server_name:");
		enviromentals.push_back(env_variable);

		// SERVER_PORT: "80" (represents the server's port number)
		// env_variable = "SERVER_PORT=9995";
		env_variable = "SERVER_PORT=" + it_req->config.port;
    	enviromentals.push_back(env_variable);

		// SERVER_PROTOCOL: "HTTP/1.1" (indicates the version of the HTTP protocol)
		env_variable = "SERVER_PROTOCOL=" + it_req->requestHeader["HTTP_version:"];
    	enviromentals.push_back(env_variable);

		// SERVER_SOFTWARE: "Apache/2.4.18" (represents the server software version)
    	env_variable = "SERVER_SOFTWARE=Weebserver";
    	enviromentals.push_back(env_variable);

		// print_enviromentals();

		char *_env[enviromentals.size() + 1];

		//turn enviroment into array
		for (std::vector<std::string>::iterator it = enviromentals.begin(); it != enviromentals.end(); it++)
		{
        	_env[i] = (char *)(*it).c_str();
			i++;
  	  	}
		_env[i] = NULL;

		// Child process
        // Set up the signal handler for timeout
		struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = handleTimeout;
        sigaction(SIGALRM, &sa, NULL);

        // Set the timeout alarm
        alarm(timeoutDuration);

		execve(_args[0], const_cast<char* const*>(_args), _env);
		throw(CgiException());
	}
	else{
		// reinstatiate the standard file descriptors

		dup2(save_stdin, STDIN_FILENO);
		close(save_stdin);
		dup2(save_stdout, STDOUT_FILENO);
		close(save_stdout);

		// Wait for the child process to exit or timeout.
		int status;
		pid_t terminatedPid = waitpid(_cgi_pid, &status, 0);

		if (terminatedPid == -1) {
			throw(CgiException());
		}

		if (timeoutOccurred) {
			// Handle timeout
			std::cout << "Timeout occurred. Child process was terminated." << std::endl;
		} else {
			// Handle normal exit
			if (WIFEXITED(status)) {
				std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
				it_req->generate_cgi_response(out_filename);
				std::cout << it_req->ResponseHeader;
			} else if (WIFSIGNALED(status)) {
				std::cout << "Child process terminated due to signal: " << WTERMSIG(status) << std::endl;
				const char* cgi_error_path = "../HTML/cgi-bin/cgi_error.html";
				it_req->generate_cgi_response(cgi_error_path);
				std::cout << it_req->ResponseHeader;
			}
		}

		//close fds
		close(outfile);	
		if (!is_query_string(it_req)){
			close(infile);
		}
	}
	
}
	
bool Cgi::is_query_string(std::vector<Request>::iterator it_req) {
    std::string key_to_find = "query_string:";
    std::map<std::string, std::string>::iterator it = it_req->requestHeader.find(key_to_find);
    return (it != it_req->requestHeader.end()) ? true : false;
}

void Cgi::print_enviromentals(){
		std::cout << "cgi: print the vector elements\n";
		for (std::vector<std::string>::iterator it = enviromentals.begin(); it != enviromentals.end(); ++it) {
			std::cout << *it << "\n";
		}
		std::cout << std::endl;
}