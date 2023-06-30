// #include "../includes/Cgi.hpp"
// #include <stdio.h>

// // //TODO catch the moment when the file is uploaded
// // //TODO have a timestamp to prevent the CGI to block the server
// // //TODO the responses of the CGI
// // //TODO check if python exists
// // //TODO check if works remoteley
// // //TODO test this if it works on a machine without python3
// // //TODO change this to const
// // //TODO a vector with the enviroment variables to creat
// // //TODO maybe a JSON file reader
// // //TODO handle post request
// // //TODO get address and port form the server setup

// // long	get_time(void)
// // {
// // 	struct timeval	tp;

// // 	gettimeofday(&tp, NULL);
// // 	return (tp.tv_sec * 1000 + tp.tv_usec / 1000);
// // }

// // void Cgi::smart_sleep(long set_miliseconds)
// // {
// // 	long start_time = get_time();
// // 	while (true)
// // 	{
// // 		if (kill(_cgi_pid, 0) != 0){
// // 			std::cerr << "process ended before kill\n";
// // 			exit(EXIT_SUCCESS);
// // 		}
// // 		// if (start_time + set_miliseconds < get_time()){
// // 		// 	std::cerr << "process killed\n";
// // 		// 	kill(_cgi_pid,9);
// // 		// 	exit(EXIT_FAILURE);
// // 		// }
// // 		usleep(5000);

// // 	}
// // }

// // bool Cgi::is_python3_installed() {
// //     const char* pythonPath = "/usr/bin/python3";
// //     if (access(pythonPath, X_OK) == 0) {
// //         return true;
// //     } else {
// //         return false;
// //     }
// // }

// // bool Cgi::is_python_file(const std::string& str) {
// //     if (str.length() < 3) {
// //         return false;
// //     }
// // 	std::string target = ".py";
// //     std::string lastThree = str.substr(str.length() - 3);
// //     return (lastThree == target);
// // }

// // void Cgi::print_request(std::map<std::string, std::string> my_map)
// // {
// //     std::map<std::string, std::string>::iterator it;
// //     switch (color_index) {
// //         case 0:
// //             for (it = my_map.begin(); it != my_map.end(); ++it) {
// //                 std::cout << RED << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
// //             }
// //             color_index++;
// //             break;
// //         case 1:
// //             for (it = my_map.begin(); it != my_map.end(); ++it) {
// //                 std::cout << YELLOW << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
// //             }
// //             color_index++;
// //             break;
// //         case 2:
// //             for (it = my_map.begin(); it != my_map.end(); ++it) {
// //                 std::cout << CYAN << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
// //             }
// //             color_index++;
// //             break;
// //         case 3:
// //             for (it = my_map.begin(); it != my_map.end(); ++it) {
// //                 std::cout << CYAN << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
// //             }
// //             color_index = 0;
// //             break;
// //         default:
// //             std::cout << "Invalid choice." << std::endl;
// //             break;
// //     }
// // }

// Cgi::Cgi(Client & c) : client(c)
// {
// 	std::stringstream string_stream;
// 	string_stream << client.id;
// 	id = string_stream.str();
// }

// Cgi::~Cgi() {}

// // std::string Cgi::get_file_name()
// // {
// // 	return (_file_name);
// // }

// void Cgi::run()
// {
// 	std::string env_variable;
// 	int infile;
// 	std::string body_path;

// 	if(!is_python3_installed())
// 		throw(CgiException());
// 	int save_stdin = dup(STDIN_FILENO);
// 	int save_stdout = dup(STDOUT_FILENO);
// 	std::cout << "cgi: request header method [" << client.request_header["method:"] << "]\n";
// 	if (!client.query_string)
// 	{
// 		const char* in_filename = body_path.c_str();
// 		infile = open(in_filename, O_RDONLY);
// 		if (infile == -1) {
// 			std::cerr << "cgi: Error opening the input file.\n";
// 			throw(CgiException());
//     	}
// 	}
	
// 	//create an output file
// 	const char* out_filename = "../HTML/cgi-bin/city_of_brass";
//     int outfile = open(out_filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
//     if (outfile == -1) {
//         std::cerr << "cgi: Error opening the outfile.\n";
//         throw(CgiException());
//     }

// 	this->_cgi_pid = fork();
// 	if (_cgi_pid < 0)
// 	{
// 		std::cerr << "Error with fork\n";
// 		throw(CgiException());
// 	}
	
// 	if (_cgi_pid == 0)
// 	{
// 		//set standard output to file
// 		dup2(outfile, STDOUT_FILENO);
// 		close(outfile);
// 		if(!client.query_string)
// 		{
// 			dup2(infile, STDIN_FILENO);
// 			close(infile);
// 		}

// 		//create arguments for execve
// 		std::string path_adder = "../HTML" + client.request_header["location:"];
// 		char* script_path = (char*)(path_adder).c_str();

// 		const char* path_to_python = "/usr/bin/python3";
//     	char* _args[3];
// 		_args[0] = (char *)path_to_python;
// 		_args[1] = script_path;
// 		_args[2] = NULL;

// 		//create enviroment
// 		std::string env_variable;

// 		int i = 0;

// 		//represents the length of the request body in bytes, only if there is a body
// 		if(!client.query_string)
// 		{
// 			env_variable = "CONTENT_LENGTH=" + client.request_header["Content-Length:"];
// 			enviromentals.push_back(env_variable);
// 			// CONTENT_TYPE: "text/html" represents the media type of the request body
// 			env_variable = "CONTENT_TYPE=" + client.request_header["Content-Type:"];
// 			enviromentals.push_back(env_variable);
// 			// PATH_INFO: "/path/to/resource" (represents the path to the requested resource)
// 			// env_variable = "PATH_INFO=../HTML/cgi-bin/ziggurat_magi_infile";
// 			env_variable = "PATH_INFO=" + client.path_info;
// 			enviromentals.push_back(env_variable);
// 		}
// 		else
// 		{
// 			env_variable = "QUERY_STRING=" + client.request_header["query_string:"];
// 			enviromentals.push_back(env_variable);	
// 		}

// 		// GATEWAY_INTERFACE: "CGI/1.1" (indicates the CGI interface version)
// 		env_variable = "GATEWAY_INTERFACE=CGI/1.1";
// 		enviromentals.push_back(env_variable);

// 		// REQUEST_METHOD: "POST" (indicates the HTTP request method)
// 		env_variable = "REQUEST_METHOD=" + client.method;
//     	enviromentals.push_back(env_variable);

// 		// REMOTE_ADDR: "192.168.0.1" (represents the IP address of the client)
// 		env_variable = "REMOTE_ADDR=" + client.client_ip;
// 		// env_variable = "REMOTE_ADDR=" + client->;
//     	enviromentals.push_back(env_variable);

// 		// SCRIPT_NAME: "/cgi-bin/script.cgi" (represents the path to the executed script)
// 		env_variable = "SCRIPT_NAME=" + client.request_header["location:"];
//     	enviromentals.push_back(env_variable);

// 		// SERVER_NAME: "example.com" (represents the server's hostname or domain name)
// 		env_variable = "SERVER_NAME=" + client.server_name;
// 		enviromentals.push_back(env_variable);

// 		// SERVER_PORT: "80" (represents the server's port number)
// 		// env_variable = "SERVER_PORT=9995";
// 		env_variable = "SERVER_PORT=" + client.config.port;
//     	enviromentals.push_back(env_variable);

// 		// SERVER_PROTOCOL: "HTTP/1.1" (indicates the version of the HTTP protocol)
// 		env_variable = "SERVER_PROTOCOL=HTTP/1.1";
//     	enviromentals.push_back(env_variable);

// 		// SERVER_SOFTWARE: "Apache/2.4.18" (represents the server software version)
//     	env_variable = "SERVER_SOFTWARE=Weebserver";
//     	enviromentals.push_back(env_variable);

// 		// print_enviromentals();

// 		char *_env[enviromentals.size() + 1];

// 		//turn enviroment into array
// 		for (std::vector<std::string>::iterator it = enviromentals.begin(); it != enviromentals.end(); it++)
// 		{
//         	_env[i] = (char *)(*it).c_str();
// 			i++;
//   	  	}
// 		_env[i] = NULL;
// 		execve(_args[0], const_cast<char* const*>(_args), _env);
// 		// exit(EXIT_SUCCESS);
// 		throw(CgiException());
// 	}
// 	// smart_sleep(3000);
// 	waitpid(-1, NULL, 0);
// 	client->generate_cgi_response(out_filename);
// 	std::cout << client->ResponseHeader;
// 	// exit(EXIT_SUCCESS);
// 	dup2(save_stdin, STDIN_FILENO);
// 	close(save_stdin);
// 	dup2(save_stdout, STDOUT_FILENO);
// 	close(save_stdout);
// 	//close fds
// 	close(outfile);
// }

// void Cgi::print_enviromentals(){
// 		std::cout << "cgi: print the vector elements\n";
// 		for (std::vector<std::string>::iterator it = enviromentals.begin(); it != enviromentals.end(); ++it) {
// 			std::cout << *it << "\n";
// 		}
// 		std::cout << std::endl;
// }