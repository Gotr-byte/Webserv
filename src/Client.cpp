#include "../includes/Client.hpp"

int Client::nextId = 0;

Client::Client(ServerConfig conf) : id(nextId++), color_index(id % 4), config(conf), header_sent(false), file_fd(-1),
content_length(0)
{
	this->autoindex = false;
	this->kill_client = false;
	this->is_cgi = false;
	this->is_get = false;
	this->cancel_recv = false;
	this->is_delete = false;
	this->is_upload = false;
	this->response_sent = false;
	this->last_chunk_sent = false;
	this->request_processed = false;
	this->request_size = 0;
}

void    Client::closeFileFd()
{
	if (file_fd != -1)
	{
		if ((close(file_fd)) < 0)
			perror("Error closing File Fd");
		else
		file_fd = -1;
	}
}

void    Client::checkRequest()
{
	this->assignLocation();
	response.server_name = config.getConfProps("server_name:");
	if (this->checkMethod() && checkExistance())
	{
		if (method == "GET")
			prepareGet();
		else if (method == "POST")
			preparePost();
		else if (method == "DELETE")
			prepareDelete();
	}
}

void	Client::prepareDelete()
{
	if (isDirectory()) //Operation is forbidden is client wants to delete a folder
	{
		setError("403");
		return;
	}
	else if (access(path_on_server.c_str(), W_OK) == -1)
	{
		setError("403");
		return;
	}
	is_delete = true;
}

void	Client::preparePost()
{
	if (std::atol(config.getConfProps("limit_body_size:").c_str()) < std::atol(request_header["Content-Length:"].c_str()))
	{
		this->cancel_recv = true;
		setError("413");
	}	
	else if (path_on_server.find(".py") != std::string::npos)
	{
		if (access(path_on_server.c_str(), X_OK) == -1)
		{
			setError("403");
			return;
		}
		this->is_cgi = true;
	}
	else if (request_header["Content-Type:"] == "multipart/form-data")
	{
		if (access(path_on_server.c_str(), W_OK) == -1)
		{
			setError("403");
			return;
		}
		this->is_upload = true;
	}
	else
		setError("403");
}

void	Client::prepareGet()
{
	this->is_get = true;
	if (path_on_server.find(".py") != std::string::npos)
	{
		if (access(path_on_server.c_str(), X_OK))
		{
			setError("403");
			return ;
		}
		this->is_cgi = true;
	}
	if (!isDirectory())
	{
		response.setResponseContentType(path_on_server);
		response.obtainFileLength(path_on_server);
	}
	else if (isDirectory() && autoindex)
		response.createAutoindex(path_on_server);
	else
		setError("403");
}

bool	Client::isDirectory()
{
	DIR *dir = opendir(path_on_server.c_str());
	if (!dir)
	{
		return false;
	}
	closedir(dir);
	return true;
}

bool	Client::checkMethod()
{
	if (config.getLocation(path_on_client, "allowed_methods:").find(request_header["method:"]) \
		!= std::string::npos)
		{
			this->method = request_header["method:"];
			return true;
		}
	setError("405");
	return false;
}

bool	Client::checkExistance()
{
	if (access(path_on_server.c_str(), F_OK) == -1)
	{
		setError("404");
		return false;
	}
	if (access(path_on_server.c_str(), R_OK) == -1)
	{
		setError("403");
		return false;
	}
	return true;
}

void	Client::assignLocation()
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

void	Client::resetProperties()
{
	this->is_cgi = false;
	this->is_get = false;
	this->is_upload = false;
	this->is_delete = false;
	this->response_sent = false;
	this->last_chunk_sent = false;
	this->request_processed = false;
}

void	Client::setError(std::string status)
{
	this->resetProperties();

	path_on_server =  config.getConfProps("error_page:") + status + ".html";
	response.error_path = path_on_server;
	
	this->request_processed = true;
	if (status == "400")
		response.setupErrorPage("400", "Bad Request");
	else if (status == "403")
		response.setupErrorPage("403", "Forbidden");
	else if (status == "404")
		response.setupErrorPage("404", "Not Found");
	else if (status == "405")
		response.setupErrorPage("405", "Method Not Allowed");
	else if (status == "409")
		response.setupErrorPage("409", "Conflict");
	else if (status == "413")
		response.setupErrorPage("413", "Payload Too Large");
	else if (status == "500")
		response.setupErrorPage("500", "Internal Server Error");
}

void    Client::mapRequestHeader()
{
	std::size_t headerEnd = request.find("\r\n\r\n");

    if (headerEnd == std::string::npos)
	{
		std::cout << "no correct header format" << std::endl;
		setError("400");
	}
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
        tokenizeRequestHeader(request_header, line);
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
}

/**
 * Tokenizes a line of text and stores the key-value pair in a map.
 *
 * @param request The map to store the key-value pair.
 * @param line_to_tokenize The line of text to tokenize.
 */
void Client::tokenizeRequestHeader(std::map<std::string, std::string> &request, std::string line_to_tokenize)
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



void	Client::setRequest(char *chunk, size_t buffer_length)
{
	request_size += buffer_length;
    for (size_t size = 0; size < buffer_length; size++)
        request.push_back(chunk[size]);
	if (buffer_length < PACKAGE_SIZE)
		request_complete = true;
}


// void Client::set_cgi_filename(Cgi &cgi){
// 	_cgi_filename = cgi.get_file_name();
// }