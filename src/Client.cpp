#include "../includes/Client.hpp"

int Client::nextId = 0;

Client::Client(SocketConfig conf, std::string ip) : config(conf), file_fd(-1), client_ip(ip)
{
	this->autoindex = false;
	this->cancel_recv = false;
	this->header_sent = false;
	this->is_cgi = false;
	this->is_delete = false;
	this->is_get = false;
	this->is_upload = false;
	this->is_redirect = false;
	this->last_chunk_sent = false;
	this->query_string = false;
	this->request_processed = false;
	this->request_size = 0;
	this->response_sent = false;
	
	std::stringstream tmp;
	tmp << nextId++;
	this->id = tmp.str();
}

Client::~Client()
{}

void	Client::setRequest(char *chunk, size_t buffer_length)
{
	request_size += buffer_length;
	if (buffer_length < PACKAGE_SIZE)
		request_complete = true;
    for (size_t size = 0; size < buffer_length; size++)
        request.push_back(chunk[size]);
}

void    Client::mapRequestHeader()
{
	std::size_t headerEnd = request.find("\r\n\r\n") + 4;

	this->header_raw = request.substr(0, headerEnd);
	request = request.substr(headerEnd);
	request_size -= headerEnd;

    std::size_t lineStart = 0;
    std::size_t lineEnd;

    lineEnd = this->header_raw.find("\r\n", lineStart);
	std::string line = header_raw.substr(lineStart, lineEnd - lineStart);
	lineStart = lineEnd + 2;

    if (!line.empty())
    {
        request_header["method:"] = std::strtok(&line[0], " ");
        request_header["location:"] = std::strtok(NULL, " ");
        std::size_t found = request_header.at("location:").find('?');
        if (found != std::string::npos && request_header.at("method:") == "GET")
		{
			this->query_string = true;
            request_header["HTTP_version:"] = std::strtok(NULL, " ");
            std::string temporary = std::strtok(&request_header.at("location:")[0], "?");
            request_header["query_string:"] = std::strtok(NULL, " ");
            request_header["location:"] = temporary;
        }
        else
            request_header["HTTP_version:"] = std::strtok(NULL, " ");
    }
	while ((lineEnd = header_raw.find("\r\n", lineStart)) != std::string::npos)
    {
		std::string line = header_raw.substr(lineStart, lineEnd - lineStart);
        tokenizeRequestHeader(request_header, line);
		lineStart = lineEnd + 2;
    }
	if (this->request_header.at("method:") == "POST" && \
		this->request_header.find("Content-Type:") == request_header.end() && \
		this->request_header.find("Content-Length:") == request_header.end())
		return;
    size_t v;
    if (this->request_header.at("method:") == "POST" && (v = request_header["Content-Type:"].find("boundary=")) != std::string::npos)
    {
        std::string key = request_header.at("Content-Type:").substr(v, 8);
        std::string value = request_header.at("Content-Type:").substr(request_header["Content-Type:"].find("=") + 1);
        request_header[key] = value;
        request_header["Content-Type:"] = request_header["Content-Type:"].substr(0, request_header["Content-Type:"].find(";"));
    }
	return;
}

void    Client::checkRequest()
{
	this->assignServer();
	this->parseClientPath();
	this->assignLocation();
	response.server_name = server_config.getConfProps("server_name:");
	if (!this->checkHeader())
		return;

	if (this->is_redirect)
		prepareRedirect();
	else if (this->checkMethod() && checkExistance())
	{
		if (method == "GET")
			prepareGet();
		else if (method == "POST")
			preparePost();
		else if (method == "DELETE")
			prepareDelete();
	}
}

bool	Client::checkHeader()
{
	size_t headerEnd = header_raw.find("\r\n\r\n");

    if (headerEnd == std::string::npos)
	{
		std::cerr << "no correct header format" << std::endl;
		setError("400");
		return false;
	}
	if (!isHeaderValid())
		return false;
	return true;
}


void	Client::setError(std::string status)
{
	this->resetProperties();

	if (status == "500")
		path_on_server = "www/error_pages/500.html";
	else
		path_on_server =  server_config.getConfProps("error_page:") + status + ".html";
	obtainFileLength();
	
	this->request_processed = true;
	if (status == "400")
		response.generateErrorResponse("400", "Bad Request");
	else if (status == "403")
		response.generateErrorResponse("403", "Forbidden");
	else if (status == "404")
		response.generateErrorResponse("404", "Not Found");
	else if (status == "405")
		response.generateErrorResponse("405", "Method Not Allowed");
	else if (status == "409")
		response.generateErrorResponse("409", "Length Required");
	else if (status == "411")
		response.generateErrorResponse("411", "Conflict");
	else if (status == "413")
		response.generateErrorResponse("413", "Payload Too Large");
	else if (status == "415")
		response.generateErrorResponse("415", "Unsupported Media Type");
	else if (status == "500")
		response.generateErrorResponse("500", "Internal Server Error");
}

bool	Client::obtainFileLength()
{
	FILE* file = fopen(path_on_server.c_str(), "r");
	if (!file)
	{
		std::cerr << ("response: Error opening file");
		setError("500");
		return false;
	}
	std::fseek(file, 0, SEEK_END);
	response.content_length = std::ftell(file);
	std::fclose(file);
	return true;
}

void    Client::closeFileFd()
{
	if (file_fd != -1)
		close(file_fd);
	file_fd = -1;
}

void	Client::prepareDelete()
{
	if (isDirectory())
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

void	Client::prepareGet()
{
	this->is_get = true;
	if (path_on_server.find(cgi_extension) != std::string::npos)
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
		obtainFileLength();
	}
	else if (isDirectory() && autoindex)
		response.createAutoindex(path_on_server);
	else
		setError("403");
}

void	Client::preparePost()
{
	if ((long)request_size < atol(request_header.at("Content-Length:").c_str()))
		request_complete = false;
	if (std::atol(server_config.getConfProps("limit_body_size:").c_str()) < std::atol(request_header["Content-Length:"].c_str()))
	{
		this->cancel_recv = true;
		setError("413");
	}	
	else if (path_on_server.find(cgi_extension) != std::string::npos)
	{
		if (access(path_on_server.c_str(), X_OK) == -1)
		{
			setError("403");
			return;
		}
		this->is_cgi = true;
	}
	else if (request_header.at("Content-Type:") == "multipart/form-data")
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

void	Client::prepareRedirect()
{
	response.generateRedirectionResponse(redirect_url);
	this->request_processed = true;
}

void	Client::parseClientPath()
{
	path_on_client = request_header.at("location:");
	size_t i,j;
	if ((i = path_on_client.find(".")) != std::string::npos && (j = path_on_client.substr(i).find("/")) != std::string::npos)
	{
		path_info = path_on_client.substr(i + j);
		path_on_client = path_on_client.substr(0, i + j);
	}
}

void	Client::assignServer()
{
	for (std::map<std::string, SocketConfig::ServerConfig>::iterator it = config.servers.begin(); it != config.servers.end(); it++)
	{
		if (it->first.find(request_header.at("Host:")) != std::string::npos)
		{
			this->server_config = it->second;
			return;
		}
		else if (it->second.first_server)
			this->server_config = it->second;
	}
}

void	Client::assignLocation()
{
	for (std::map<std::string, std::map<std::string, std::string> >::iterator \
		it = server_config.locations.begin(); it != server_config.locations.end(); it++)
	{
		if (path_on_client.find(it->first) != std::string::npos)
		{
			this->location = it->first;
			if (it->second.find("redirect:") != it->second.end())
			{
				redirect_url = it->second.at("redirect:");
				this->is_redirect = true;
				return;
			}
			if (it->second.find("cgi_path:") != it->second.end() && it->second.find("cgi_path:") != it->second.end())
			{
				cgi_path = it->second.at("cgi_path:");
				cgi_extension = it->second.at("cgi_ext:");
			}
			this->path_on_server = it->second.at("root:");
			if (path_on_client == it->first)
				this->path_on_server += it->second["index:"];
			else
				this->path_on_server += path_on_client.substr(it->first.size());
			if (it->second.at("autoindex:") == "on")
				autoindex = true;
		}
	}
}

bool	Client::checkMethod()
{
	if (server_config.getLocation(location, "allowed_methods:").find(request_header["method:"]) \
		!= std::string::npos)
		{
			this->method = request_header.at("method:");
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

void	Client::resetProperties()
{
	this->header_sent = false;
	this->is_cgi = false;
	this->is_delete = false;
	this->is_get = false;
	this->is_upload = false;
	this->last_chunk_sent = false;
	this->request_processed = false;
	this->response_sent = false;
}

bool	Client::isHeaderValid()
{
	if (this->request_header.at("method:") == "POST")
	{
		if (this->request_header.find("Content-Type:") == request_header.end())
		{
			setError("403");
			return false;
		}
		else if (this->request_header.at("Content-Type:").find("application/x-www-form-urlencoded") == std::string::npos && \
					this->request_header.at("Content-Type:").find("multipart/form-data") == std::string::npos)
		{
			setError("415");
			return false;
		}
		if (this->request_header.find("Content-Length:") == request_header.end())
		{
			setError("411");
			return false;
		}
	}
	return true;
}

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
