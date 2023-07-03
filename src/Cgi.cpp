#include "../includes/Cgi.hpp"


#define WRITE_END 1
#define READ_END 0

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

Cgi::Cgi(Client & c) : client(c)
{
	std::stringstream string_stream;
	string_stream << client.id;
	id = string_stream.str();
}

Cgi::~Cgi() {}

// std::string Cgi::get_file_name()
// {
// 	return (_file_name);
// }

void Cgi::run()
{
	std::string env_variable;
	int infile;
	std::string body_path;

	const int timeoutDuration = 3;

	if(!is_python3_installed())
		throw(CgiException());
	std::cout << "cgi: request header method [" << client.request_header["method:"] << "]\n";

	//create an output file
	const char* out_filename = "../HTML/cgi-bin/city_of_brass"; //dynamic filename 
    int outfile = open(out_filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (outfile == -1) 
	{
        std::cerr << "cgi: Error opening the outfile.\n";
        throw(CgiException());
    }

	int pipe_d[2];
	if (pipe(pipe_d) == -1)
		std::cout << "Pipe Error\n";
	
    if (client.request.size() > 0)
        fcntl(pipe_d[WRITE_END], 0, client.request.size());

    write(pipe_d[WRITE_END], client.request.c_str(), client.request.size());
    close(pipe_d[WRITE_END]);

	int _cgi_pid = fork();
	if (_cgi_pid < 0)
	{
		close(pipe_d[READ_END]);
		close(outfile);
		std::cerr << "Error with fork\n";
		throw(CgiException());
	}
	
	if (_cgi_pid == 0)
	{
		//set standard output to file
		dup2(outfile, STDOUT_FILENO);
		close(outfile);
		if(!client.query_string)
		{
			dup2(infile, STDIN_FILENO);
			close(infile);
		}

		//create arguments for execve
		std::string path_adder = "../HTML" + client.request_header["location:"];
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
		if(!client.query_string)
		{
			env_variable = "CONTENT_LENGTH=" + client.request_header["Content-Length:"];
			enviromentals.push_back(env_variable);
			// CONTENT_TYPE: "text/html" represents the media type of the request body
			env_variable = "CONTENT_TYPE=" + client.request_header["Content-Type:"];
			enviromentals.push_back(env_variable);
			// PATH_INFO: "/path/to/resource" (represents the path to the requested resource)
			env_variable = "PATH_INFO=" + client.path_info;
			enviromentals.push_back(env_variable);
		}
		else
		{
			env_variable = "QUERY_STRING=" + client.request_header["query_string:"];
			enviromentals.push_back(env_variable);	
		}

		// GATEWAY_INTERFACE: "CGI/1.1" (indicates the CGI interface version)
		env_variable = "GATEWAY_INTERFACE=CGI/1.1";
		enviromentals.push_back(env_variable);

		// REQUEST_METHOD: "POST" (indicates the HTTP request method)
		env_variable = "REQUEST_METHOD=" + client.method;
    	enviromentals.push_back(env_variable);

		// REMOTE_ADDR: "192.168.0.1" (represents the IP address of the client)
		env_variable = "REMOTE_ADDR=" + client.client_ip;
    	enviromentals.push_back(env_variable);

		// SCRIPT_NAME: "/cgi-bin/script.cgi" (represents the path to the executed script)
		env_variable = "SCRIPT_NAME=" + client.request_header["location:"];
    	enviromentals.push_back(env_variable);

		// SERVER_NAME: "example.com" (represents the server's hostname or domain name)
		env_variable = "SERVER_NAME=" + client.server_name;
		enviromentals.push_back(env_variable);

		// SERVER_PORT: "80" (represents the server's port number)
		env_variable = "SERVER_PORT=" + client.config.port;
    	enviromentals.push_back(env_variable);

		// SERVER_PROTOCOL: "HTTP/1.1" (indicates the version of the HTTP protocol)
		env_variable = "SERVER_PROTOCOL=HTTP/1.1";
    	enviromentals.push_back(env_variable);

		// SERVER_SOFTWARE: "Apache/2.4.18" (represents the server software version)
    	env_variable = "SERVER_SOFTWARE=Weebserver";
    	enviromentals.push_back(env_variable);

		char *_env[enviromentals.size() + 1];

		//turn enviroment into array
		for (std::vector<std::string>::iterator it = enviromentals.begin(); it != enviromentals.end(); it++)
		{
        	_env[i] = (char *)(*it).c_str();
			i++;
  	  	}
		_env[i] = NULL;

		dup2(pipe_d[READ_END], STDIN_FILENO);
        close(pipe_d[READ_END]);

        dup2(outfile, STDOUT_FILENO);
        close(outfile);
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

	int status;
	close(pipe_d[READ_END]);
    close(outfile);
		pid_t terminatedPid = waitpid(_cgi_pid, &status, 0);
		if (terminatedPid == -1) {
			std::cerr << "cgi: error with process handling\n";
			throw(CgiException());
		}

		if (timeoutOccurred) {
			// Handle timeout
			std::cerr << "Timeout occurred. Child process was terminated." << std::endl;
		} else {
			// Handle normal exit
			if (WIFEXITED(status)) {
				// std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
				client.path_on_server = out_filename;
				client.response.generateCgiResponse(out_filename);
			} else if (WIFSIGNALED(status)) {
				std::cerr << "Child process terminated due to signal: " << WTERMSIG(status) << std::endl;
				const char* cgi_error_path = "../HTML/cgi-bin/cgi_error.html";
				client.response.generateCgiResponse(out_filename);
				throw(CgiException());
			}
		}
}

void Cgi::print_enviromentals(){
		std::cout << "cgi: print the vector elements\n";
		for (std::vector<std::string>::iterator it = enviromentals.begin(); it != enviromentals.end(); ++it) {
			std::cout << *it << "\n";
		}
		std::cout << std::endl;
}