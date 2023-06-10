#include "../includes/Request.hpp"

int Request::nextId = 0;

Request::Request() : id(nextId++), initialResponseSent(false), file_fd(-1), client_fd(-1), requestdone(false), error(false)
{}

void	Request::CreateResponse(ServerConfig	conf)
{
	contenttype = "application/octet-stream";
	statuscode = "200 OK";
	cutoffClient = false,
	isCGI = false;
	isUpload = false;
	config = conf;
	isdirectory = false;
	autoindex = false;
	protocoll = "HTTP/1.1";
	additionalinfo = "Transfer-Encoding: chunked";

	this->AssignLocation();
	if (this->CheckMethod() && this->CheckPath())
	{
		SetContentType();
		ObtainFileLength();
	}
	this->setDate();
	this->BuildResponseHeader();
}

void	Request::GenerateOverloadError(int errorcode, ServerConfig	conf)
{
	cutoffClient = true;
	config = conf;
	protocoll = "HTTP/1.1", \
	additionalinfo = "Connection: closed\nTransfer-Encoding: chunked";

	if (errorcode == 503)
		SetupErrorPage("503", "Unavailable");
	this->setDate();
	this->BuildResponseHeader();
}

void	Request::AssignLocation()
{
	for (std::map<std::string, std::map<std::string, std::string> >::iterator \
		it = config.locations.begin(); it != config.locations.end(); it++)
	{
		// std::cout << it->first << std::endl; 
		if (int pos = requestHeaderMap["location:"].find(it->first) != std::string::npos)
		{
			this->clientpath = it->first;
			this->path = it->second["root:"];
			if (requestHeaderMap["location:"] == it->first)
				this->path += it->second["index:"];
			else
				this->path += requestHeaderMap["location:"].substr(1);
			if (it->second["autoindex:"] == "on")
				autoindex = true;
		}
	}
	// std::cout << "CORRECT PATH: " << path << std::endl;
}

bool	Request::CheckMethod()
{
	if (config.getLocation(clientpath, "allowed_methods:").find(requestHeaderMap["method:"]) \
		!= std::string::npos)
		{
			this->method = requestHeaderMap["method:"];
			return true;
		}
	SetupErrorPage("405", "Method Not Allowed");
	return false;
}

bool	Request::CheckPath()
{
	if (CheckExistance())
	{
		if (!IsDirectory())
			return true;
		else
		{
			if (method == "POST" || method == "DELETE")
				return CheckPermissions() && CheckBodySize();
			else if (!autoindex)
			{
				SetupErrorPage("403", "Forbidden");
				return false;
			}
			CreateAutoindex();
		}
	}
	return false;
}

bool	Request::CheckBodySize()
{
	// std::cout << config.getConfProps("limit_body_size:") << requestHeaderMap["Content-Length:"] << std::endl;
	if (method == "POST")
	{
		setPostType();
		if (std::atol(config.getConfProps("limit_body_size:").c_str()) < std::atol(requestHeaderMap["Content-Length:"].c_str()))
		{
			SetupErrorPage("413", "Payload Too Large");
			return false;
		}
	}
	return true;
}

void	Request::setPostType()
{
	if (requestHeaderMap["Content-Type:"] == "multipart/form-data")
		this->isUpload = true;
	else
		this->isCGI = true;
}

bool	Request::CheckPermissions()
{
	if (access(path.c_str(), W_OK) == -1)
	{
		SetupErrorPage("403", "Forbidden");
		return false;
	}
	if (requestHeaderMap["Content-Type:"] != "multipart/form-data" && access(path.c_str(), X_OK) == -1)
	{
		SetupErrorPage("403", "Forbidden");
		return false;
	}
	return true;
}

void	Request::SetupErrorPage(std::string status, std::string issue)
{
	path = "../error_pages/" + status + ".html";
	statuscode = status + " " + issue;
	contenttype = "text/html";
	if (status == "413" || status == "503")
		cutoffClient = true;
	ObtainFileLength();
	// std::cout << path << std::endl;
	// std::cout << statuscode << std::endl;
	// std::cout << contenttype << std::endl;
	// std::cout << method << std::endl;
}

void	Request::ObtainFileLength()
{
	FILE* file = fopen(path.c_str(), "r");
	if (file == nullptr)	
	{
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	std::fseek(file, 0, SEEK_END);
	contentlength = std::ftell(file);
	std::fclose(file);
}

bool	Request::CheckExistance()
{
	if (access(path.c_str(), F_OK) == -1)
	{
		SetupErrorPage("404", "Not Found");
		return false;
	}
	if (access(path.c_str(), R_OK) == -1)
	{
		SetupErrorPage("403", "Forbidden");
		return false;
	}
	return true;
}

bool	Request::IsDirectory()
{
	DIR *dir = opendir(path.c_str());
	if (!dir)
	{
		return false;
	}
	closedir(dir);
	return true;
}

void	Request::setDate()
{
	std::time_t currentTime = std::time(nullptr);
    std::tm* timeinfo = std::gmtime(&currentTime);
    
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    
    std::string conv(buffer);
	this->date = "Date: " + conv;
	// std::cout << date << std::endl;
}

void	Request::SetContentType()
{
	if (method == "GET")
	{
		std::string suffix = path.substr(path.rfind(".") + 1);

		if (path.find("/file/") != std::string::npos)
			contenttype = "application/octet-stream";
		else
		{
			if (suffix == "html")
				contenttype = "text/html";
			else if (suffix == "css")
				contenttype = "text/css";
			else if (suffix == "txt")
				contenttype = "text/plain";
			else if (suffix == "ico")
				contenttype = "image/x-icon";
			else if (suffix == "jpg" || suffix == "jpeg")
				contenttype = "image/jpeg";
			else if (suffix == "png")
				contenttype = "image/png";
			else if (suffix == "gif")
				contenttype = "image/gif";
			else if (suffix == "pdf")
				contenttype = "application/pdf";
			else if (suffix == "mp3")
				contenttype = "audio/mpeg";
			else if (suffix == "mp4")
				contenttype = "audio/mpeg";
			else
				contenttype = "application/octet-stream";
		}

	}
	// std::cout << suffix << std::endl;
}

void	Request::BuildResponseHeader()
{
	std::ostringstream header;

	header << protocoll << " " << statuscode << "\r\n";
	header << "Server: " << config.getConfProps("server_name:") << "\r\n";
	header << date << "\r\n";
	header << "Content-Type: " << contenttype << "\r\n";
	header << "Content-Length: " << contentlength << "\r\n";
	if (!additionalinfo.empty())
		header << additionalinfo << "\r\n";
	header << "\r\n";

	ResponseHeader = header.str();
	std::cout << ResponseHeader << std::endl;
}

void	Request::CreateAutoindex()
{
	std::ostringstream	autoidx;

	autoidx << "<!DOCTYPE html>\n";
    autoidx << "<html>\n";
    autoidx << "<head>\n";
    autoidx << "<title>Index of " << path << "</title>\n";
    autoidx << "</head>\n";
    autoidx << "<body>\n";
    autoidx << "<h1>Index of " << path << "</h1>\n";
    autoidx << "<ul>\n";

	DIR* dir = opendir(path.c_str());
	// std::cout << path << std::endl;

	struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." entries
        if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
            continue;
        }
        
        std::string itemName = entry->d_name;
        std::string itemPath = path + "/" + itemName;
        
        // Check if the entry is a file or directory
        bool isDir = (entry->d_type == DT_DIR);
        
        // Generate the appropriate HTML entry
        if (isDir)
            autoidx << "<li><a href=\"" << itemName << "/\">" << itemName << "/</a></li>\n";
        else
            autoidx << "<li><a href=\"" << itemName << "\">" << itemName << "</a></li>\n";
        }
    
    // Close the directory
    closedir(dir);
    
    // Write the HTML footer
    autoidx << "</ul>\n";
    autoidx << "</body>\n";
    autoidx << "</html>\n";

	autoindexbody = autoidx.str();
	additionalinfo = "";
	path = "AUTOINDEX";
	contenttype = "text/html";
	contentlength = autoindexbody.size();
	// std::cout << autoindexbody << std::endl;
}

