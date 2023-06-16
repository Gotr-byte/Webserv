#include "../includes/Cgi.hpp"
#include <stdio.h>

//TODO catch the moment when the file is uploaded
//TODO have a timestamp to prevent the CGI to block the server
//TODO the responses of the CGI
//TODO check if python exists
//TODO check if works remoteley
//TODO test this if it works on a machine without python3
//TODO change this to const
//TODO a vector with the enviroment variables to creat
//TODO maybe a JSON file reader
//TODO handle post request
	//TEST
		// MUST
		// _env[0] = const_cast<char*>("CONTENT_TYPE=text/html");  // Example value for Content-Type
		// _env[1] = const_cast<char*>("GATEWAY_INTERFACE=CGI/1.1");
		// _env[2] = const_cast<char*>("PATH_TRANSLATED=../HTML/cgi-bin/ziggurat_magi.py");
		// _env[3] = const_cast<char*>("REQUEST_METHOD=POST");
		// _env[4] = const_cast<char*>("CONTENT_LENGTH=1024");
		// _env[5] = const_cast<char*>("SERVER_SOFTWARE=Weebserver");
		// _env[6] = const_cast<char*>("SERVER_NAME=Weebserver.com");  // Example value for server name
		// _env[7] = const_cast<char*>("HTTP_ACCEPT=text/html");  // Example value for Accept header
		// _env[8] = const_cast<char*>("HTTP_ACCEPT_LANGUAGE=en-US");  // Example value for Accept-Language header
		// _env[9] = const_cast<char*>("HTTP_USER_AGENT=Mozilla/5.0");  // Example value for User-Agent header
		// _env[10] = const_cast<char*> ("SCRIPT_NAME=../HTML/cgi-bin/print_params.py");  // Example value for script name
		// _env[11] = const_cast<char*>("HTTP_REFERER=http://example.com"); 
		// _env[12] = const_cast<char*>("PATH_TRANSLATED=../HTML/cgi-bin/print_params.py");
		// _env[13] = const_cast<char*>("QUERY_STRING=param1=value3&param2=value4");
		// _env[14] = const_cast<char*>("REQUEST_METHOD=POST");

		// _env[15] = const_cast<char*>("CONTENT_LENGTH=512"); //important one, it shows body lenght
		// _env[16] = const_cast<char*> ("SERVER_SOFTWARE=MyWeebserver");
		// _env[17] = NULL;

		//END OF TEST

	// 	Servers MUST provide the following 12 meta-variables to scripts. See the individual descriptions for exceptions and semantics.

    // CONTENT_LENGTH
    // CONTENT_TYPE
    // GATEWAY_INTERFACE
    // PATH_INFO
    // QUERY_STRING
    // REQUEST_METHOD
    // REMOTE_ADDR
    // SCRIPT_NAME
    // SERVER_NAME
    // SERVER_PORT
    // SERVER_PROTOCOL
    // SERVER_SOFTWARE

	// CONTENT_LENGTH: "1024" (represents the length of the request body in bytes, only if there is a body)
	// CONTENT_TYPE: "text/html" (represents the media type of the request body)
	// GATEWAY_INTERFACE: "CGI/1.1" (indicates the CGI interface version)
	// PATH_INFO: "/path/to/resource" (represents the path to the requested resource)
	// QUERY_STRING: "param1=value1&param2=value2" (represents the query string parameters)
	// REQUEST_METHOD: "POST" (indicates the HTTP request method)
	// REMOTE_ADDR: "192.168.0.1" (represents the IP address of the client)
	// SCRIPT_NAME: "/cgi-bin/script.cgi" (represents the path to the executed script)
	// SERVER_NAME: "example.com" (represents the server's hostname or domain name)
	// SERVER_PORT: "80" (represents the server's port number)
	// SERVER_PROTOCOL: "HTTP/1.1" (indicates the version of the HTTP protocol)
	// SERVER_SOFTWARE: "Apache/2.4.18" (represents the server software version)

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
		if (read_bytes < 0)
			break;
		outFile.write(buf, read_bytes);
		remaining_bytes -= chunk_bytes;
	}
	outFile.close();
    memset(buf, 0, BUF_SIZE);
    recv(it_req->client_fd, buf, BUF_SIZE, MSG_DONTWAIT);
	return (filename.str());
}

// Use try while running
void Cgi::run(std::vector<Request>::iterator it_req)
{
	std::string env_variable;

	if(!is_python3_installed())
		throw(CgiException());
	if(!is_python_file(it_req->requestHeader["location:"]))
		throw(CgiException());
	if (it_req->method == "POST")
		std::string body_path = create_request_body_file(it_req);
	it_req->GenerateUploadResponse();
	this->_cgi_pid = fork();
	if (_cgi_pid < 0)
	{
		std::cerr << "Error with fork\n";
		throw(CgiException());
	}
	//create a file
	const char* out_filename = "../HTML/cgi-bin/city_of_brass";
    int outfile = open(out_filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (outfile == -1) {
        std::cerr << "cgi: Error opening the outfile.\n";
        throw(CgiException());
    }

	// const char* in_filename = "../HTML/cgi-bin/ziggurat_magi_infile";
    // int in_file = open(in_filename, O_RDONLY);
    // if (in_file == -1) {
    //     std::cerr << "cgi: Error opening the input file.\n";
    //     throw(CgiException());
    // }

	if (_cgi_pid == 0)
	{
		//set standard output to file
		dup2(outfile, STDOUT_FILENO);
		close(outfile);

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
		env_variable = "CONTENT_LENGTH=7";
		enviromentals.push_back(env_variable);

		// CONTENT_TYPE: "text/html" represents the media type of the request body
		env_variable = "CONTENT_TYPE=" + it_req->requestHeader["Content-Type:"];
		enviromentals.push_back(env_variable);

		// GATEWAY_INTERFACE: "CGI/1.1" (indicates the CGI interface version)
		env_variable = "GATEWAY_INTERFACE=CGI/1.1";
		enviromentals.push_back(env_variable);

		// PATH_INFO: "/path/to/resource" (represents the path to the requested resource)
		env_variable = "PATH_INFO=../HTML/cgi-bin/ziggurat_magi_infile";
		enviromentals.push_back(env_variable);

		// QUERY_STRING: "param1=value1&param2=value2" (represents the query string parameters)
		// in case of POST this should be read from file
		std::string keyToFind = "query_string:";
		std::map<std::string, std::string>::iterator it = it_req->requestHeader.find(keyToFind);
		if (it != it_req->requestHeader.end()) {
			env_variable = "QUERY_STRING=" + it_req->requestHeader["query_string:"];
			enviromentals.push_back(env_variable);	
		}
		// env_variable = "QUERY_STRING=farm=13";
		// enviromentals.push_back(env_variable);

		// REQUEST_METHOD: "POST" (indicates the HTTP request method)
		// env_variable = "REQUEST_METHOD=" + it_req->requestHeader["method:"];
		env_variable = "REQUEST_METHOD=POST";
    	enviromentals.push_back(env_variable);

		// REMOTE_ADDR: "192.168.0.1" (represents the IP address of the client)
		env_variable = "REMOTE_ADDR=127.0.0.1";
    	enviromentals.push_back(env_variable);

		// SCRIPT_NAME: "/cgi-bin/script.cgi" (represents the path to the executed script)
		env_variable = "SCRIPT_NAME=" + it_req->requestHeader["location:"];
    	enviromentals.push_back(env_variable);

		// SERVER_NAME: "example.com" (represents the server's hostname or domain name)
		env_variable = "SERVER_NAME=Weebserver.com";
		enviromentals.push_back(env_variable);

		// SERVER_PORT: "80" (represents the server's port number)
		env_variable = "SERVER_PORT=9995";
    	enviromentals.push_back(env_variable);

		// SERVER_PROTOCOL: "HTTP/1.1" (indicates the version of the HTTP protocol)
		env_variable = "SERVER_PROTOCOL=" + it_req->requestHeader["HTTP_version:"];
    	enviromentals.push_back(env_variable);

		// SERVER_SOFTWARE: "Apache/2.4.18" (represents the server software version)
    	env_variable = "SERVER_SOFTWARE=Weebserver";
    	enviromentals.push_back(env_variable);

		std::cout << "cgi: print the vector elements\n";
		for (std::vector<std::string>::iterator it = enviromentals.begin(); it != enviromentals.end(); ++it) {
			std::cout << *it << "\n";
		}
		std::cout << std::endl;

		char *_env[enviromentals.size() + 1];
		std::cout << "cgi: enviromentals size equals " << enviromentals.size() << "\n";

		//turn enviroment into array
		for (std::vector<std::string>::iterator it = enviromentals.begin(); it != enviromentals.end(); it++)
		{
        	_env[i] = (char *)(*it).c_str();
			i++;
  	  	}
		_env[i] = NULL;
		execve(_args[0], const_cast<char* const*>(_args), _env);
		throw(CgiException());
	}
	//close fds
	close(outfile);
}
	
