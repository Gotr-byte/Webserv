#include "../includes/Client.hpp"

int Client::nextId = 0;

Client::Client(int server_idx, ServerConfig conf) : id(nextId++), color_index(id % 4) , server_index(server_idx), config(conf), header_sent(false), file_fd(-1),
content_length(0), is_error(false)
{
	kill_client = false;
	isCGI = false;
	isDelete = false;
	isUpload = false;
	response_sent = false;
	send_last_chunk = false;
	request_processed = false;
	request_size = 0;
}

void    Client::close_file_fd()
{
	if (file_fd != -1)
	{
		if ((close(file_fd)) < 0)
			perror("Error closing File Fd");
		else
			std::cout << "CLOSED FILE FD: " << file_fd << std::endl;
		file_fd = -1;
	}
}

void    Client::create_response()
{
	response.CreateResponse(config);
}

void    Client::check_request()
{
	this->assign_location();
	if (this->check_method() && check_existance())
	{
		response.server_name = config.getConfProps("server_name:");
		if (method == "GET")
		{
			prepare_get();
		}
		else if (method == "POST")
			prepare_post();
		else if (method == "DELETE")
			prepare_delete();
	}
}

void	Client::prepare_delete()
{
	if (is_directory()) //Operation is forbidden is client wants to delete a folder
	{
		set_error("403");
		return;
	}
	else if (access(path_on_server.c_str(), W_OK) == -1)
	{
		set_error("403");
		return;
	}
	isDelete = true;
}

void	Client::prepare_post()
{
	if (std::atol(config.getConfProps("limit_body_size:").c_str()) < std::atol(request_header["Content-Length:"].c_str()))
	{
		set_error("413");
	}	
	else if (path_on_server.find(".py") != std::string::npos)
	{
		if (access(path_on_server.c_str(), X_OK) == -1)
		{
			set_error("403");
			return;
		}
		this->isCGI = true;
	}
	else if (request_header["Content-Type:"] == "multipart/form-data")
	{
		if (access(path_on_server.c_str(), W_OK) == -1)
		{
			set_error("403");
			return;
		}
		this->isUpload = true;
	}
	else
		set_error("403");
}

void	Client::prepare_get()
{
	if (path_on_server.find(".py") != std::string::npos)
	{
		if (access(path_on_server.c_str(), X_OK))
		{
			set_error("403");
			return ;
		}
		this->isCGI = true;
	}
	if (!is_directory())
	{
		response.SetResponseContentType(path_on_server);
		response.ObtainFileLength(path_on_server);
	}
	else if (is_directory() && autoindex)
		response.CreateAutoindex(path_on_server);
	else
		set_error("403");
}

bool	Client::is_directory()
{
	DIR *dir = opendir(path_on_server.c_str());
	if (!dir)
	{
		return false;
	}
	closedir(dir);
	return true;
}

bool	Client::check_method()
{
	if (config.getLocation(path_on_client, "allowed_methods:").find(request_header["method:"]) \
		!= std::string::npos)
		{
			this->method = request_header["method:"];
			return true;
		}
	set_error("405");
	return false;
}

bool	Client::check_existance()
{
	if (access(path_on_server.c_str(), F_OK) == -1)
	{
		set_error("404");
		return false;
	}
	if (access(path_on_server.c_str(), R_OK) == -1)
	{
		set_error("403");
		return false;
	}
	return true;
}

void	Client::assign_location()
{
	for (std::map<std::string, std::map<std::string, std::string> >::iterator \
		it = config.locations.begin(); it != config.locations.end(); it++)
	{
		// std::cout << it->first << std::endl; 
		if (int pos = request_header["location:"].find(it->first) != std::string::npos)
		{
			this->path_on_client = it->first;
			this->path_on_server = it->second["root:"];
			if (request_header["location:"] == it->first)
				this->path_on_server += it->second["index:"];
			else
				this->path_on_server += request_header["location:"].substr(1);
			if (it->second["autoindex:"] == "on")
				autoindex = true;
		}
	}
}

void	Client::set_error(std::string status)
{
	path_on_server =  config.getConfProps("error_page:") + status + ".html";
	response.error_path = path_on_server;
	
	this->is_error = true;
	if (status == "403")
		response.SetupErrorPage("403", "Forbidden");
	else if (status == "404")
		response.SetupErrorPage("404", "Not Found");
	else if (status == "405")
		response.SetupErrorPage("405", "Method Not Allowed");
	else if (status == "409")
		response.SetupErrorPage("409", "Conflict");
	else if (status == "413")
		response.SetupErrorPage("413", "Payload Too Large");
	else if (status == "500")
		response.SetupErrorPage("500", "Internal Server Error");
}

void    Client::mapping_request_header()
{
    int new_line_count = 0;
    // Get lines of Header
    int n;

	std::size_t headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
		perror("no correct header format");
        // Handle error: invalid HTTP request without a 

	std::string header = request.substr(0, headerEnd);
	request = request.substr(headerEnd + 4);

	request_size -= headerEnd + 4;

    std::size_t lineStart = 0;
    std::size_t lineEnd;

    lineEnd = header.find("\r\n", lineStart);
	std::string line = header.substr(lineStart, lineEnd - lineStart);
	lineStart = lineEnd + 2;

	 // Extract a line from the header
    if (!line.empty())
    {
        request_header["method:"] = std::strtok(&line[0], " ");
        request_header["location:"] = std::strtok(NULL, " ");
        std::size_t found = request_header["location:"].find('?');
        if (found != std::string::npos && request_header["method:"] == "GET")
		{
            request_header["HTTP_version:"] = std::strtok(NULL, " ");
            std::string temporary = std::strtok(&request_header["location:"][0], "?");
            request_header["query_string:"] = std::strtok(NULL, " ");
            request_header["location:"] = temporary;
        }
        else
            request_header["HTTP_version:"] = std::strtok(NULL, " ");
    }
	while ((lineEnd = header.find("\r\n", lineStart)) != std::string::npos)
    {
		std::string line = header.substr(lineStart, lineEnd - lineStart);
        tokenizing(request_header, line);
		lineStart = lineEnd + 2;
    }
    size_t v;
    if ((v = request_header["Content-Type:"].find("boundary=")) != std::string::npos)
    {
        std::string key = request_header["Content-Type:"].substr(v, 8);
        std::string value = request_header["Content-Type:"].substr(request_header["Content-Type:"].find("=") + 1);
        request_header[key] = value;
        request_header["Content-Type:"] = request_header["Content-Type:"].substr(0, request_header["Content-Type:"].find(";"));
    }
    // print_request(request_header);
}

void Client::print_request(std::map<std::string, std::string> my_map)
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

/**
 * Tokenizes a line of text and stores the key-value pair in a map.
 *
 * @param request The map to store the key-value pair.
 * @param line_to_tokenize The line of text to tokenize.
 */
void Client::tokenizing(std::map<std::string, std::string> &request, std::string line_to_tokenize)
{
    std::stringstream tokenize_stream(line_to_tokenize);
    std::string value;
    std::string key;
    key = line_to_tokenize.substr(0, line_to_tokenize.find(":") + 1);
    value = line_to_tokenize.substr(line_to_tokenize.find(":") + 1);
    removeWhitespaces(key);
    removeWhitespaces(value);
    request[key] = value;
}

void Client::removeWhitespaces(std::string &string)
{
    string.erase(0, string.find_first_not_of(" \t"));
    string.erase(string.find_last_not_of(" \t") + 1);
}



void	Client::set_request(char *chunk, size_t buffer_length)
{
	request_size += buffer_length;
    for (size_t size = 0; size < buffer_length; size++)
        request.push_back(chunk[size]);
	if (request_size < PACKAGE_SIZE)
	{
		request_complete = true;
	}
}


// void Client::set_cgi_filename(Cgi &cgi){
// 	_cgi_filename = cgi.get_file_name();
// }