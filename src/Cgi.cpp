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

// Use try while running
void Cgi::run(std::map<std::string, std::string> request)
{
	std::string env_variable;

	if(!is_python3_installed())
		throw(CgiException());
	if(!is_python_file(request["location:"]))
		throw(CgiException());
	this->_cgi_pid = fork();
	if (_cgi_pid < 0)
	{
		std::cerr << "Error with fork\n";
		throw(CgiException());
	}
	//create a file
	const char* filename = "../HTML/cgi-bin/city_of_brass";
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
        const char* _args[] = { "/usr/bin/python3", script_path, NULL };

		//create enviroment
		std::string end_variable;
		int variables_number = 3;
		char *_env[variables_number + 1];

		int i = 0;
		env_variable = "REQUEST_METHOD=" + request["method:"];
    	enviromentals.push_back(env_variable);
    	env_variable = "SCRIPT_NAME=" + request["location:"];
    	enviromentals.push_back(env_variable);
    	env_variable = "SERVER_PROTOCOL=" + request["HTTP_version:"];
    	enviromentals.push_back(env_variable);
    	env_variable = "SERVER_SOFTWARE=Weebserver";
    	enviromentals.push_back(env_variable);

		//looking form parameters to send
		std::string keyToFind = "query_string:";
		std::map<std::string, std::string>::iterator it = request.find(keyToFind);
		if (it != request.end()) {
			env_variable = "QUERY_STRING=" + request["query_string:"];
			enviromentals.push_back(env_variable);	
		}
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
	

	//TEST
		// _env[0] = const_cast<char*>("CONTENT_TYPE=text/html");  // Example value for Content-Type
		// _env[1] = const_cast<char*>("GATEWAY_INTERFACE=CGI/1.1");
		// _env[2] = const_cast<char*>("PATH_TRANSLATED=../HTML/cgi-bin/print_params.py");
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
		// _env[15] = const_cast<char*>("CONTENT_LENGTH=512");
		// _env[16] = const_cast<char*> ("SERVER_SOFTWARE=MyWeebserver");
		// _env[17] = NULL;

		//END OF TEST