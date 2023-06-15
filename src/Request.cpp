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
	isDelete = false;
	isUpload = false;
	config = conf;
	isdirectory = false;
	autoindex = false;
	protocoll = "HTTP/1.1";
	additionalinfo = "Transfer-Encoding: chunked";

	this->AssignLocation();
	if (this->CheckMethod() && CheckExistance())
	{
		if (method == "GET")
			PrepareGetResponse();
		else if (method == "POST")
			PreparePost();
		else if (method == "DELETE")
			PrepareDelete();
	}
	if ((method == "GET" && !isCGI) || error)
	{
		this->setDate();
		this->BuildResponseHeader();
	}
}

void	Request::PrepareDelete()
{
	if (IsDirectory()) //Operation is forbidden is client wants to delete a folder
	{
		SetupErrorPage("403", "Forbidden");
		return;
	}
	else if (access(path.c_str(), W_OK) == -1)
	{
		SetupErrorPage("403", "Forbidden");
		return;
	}
	isDelete = true;
}

void	Request::PreparePost()
{
	if (std::atol(config.getConfProps("limit_body_size:").c_str()) < std::atol(requestHeader["Content-Length:"].c_str()))
		SetupErrorPage("413", "Payload Too Large");
	else if (path.find(".py") != std::string::npos)
	{
		if (access(path.c_str(), X_OK) == -1)
		{
			SetupErrorPage("403", "Forbidden");
			return;
		}
		this->isCGI = true;
	}
	else if (requestHeader["Content-Type:"] == "multipart/form-data")
	{
		if (access(path.c_str(), W_OK) == -1)
		{
			SetupErrorPage("403", "Forbidden");
			return;
		}
		this->isUpload = true;
	}
	else
		SetupErrorPage("403", "Forbidden");
}

void	Request::PrepareGetResponse()
{
	if (path.find(".py") != std::string::npos)
	{
		if (access(path.c_str(), X_OK))
		{
			SetupErrorPage("403", "Forbidden");
			return ;
		}
		this->isCGI = true;
	}
	if (!IsDirectory())
	{
		SetResponseContentType();
		ObtainFileLength();
	}
	else if (IsDirectory() && autoindex)
		CreateAutoindex();
	else
		SetupErrorPage("403", "Forbidden");
}

void	Request::GenerateClientErrorResponse(std::string status, std::string issue)
{
	protocoll = "HTTP/1.1", \
	additionalinfo = "Connection: closed\nTransfer-Encoding: chunked";
	SetupErrorPage(status, issue);
	setDate();
	BuildResponseHeader();
}

void	Request::GenerateServerErrorResponse(int errorcode, ServerConfig	conf)
{
	config = conf;
	protocoll = "HTTP/1.1", \
	additionalinfo = "Connection: closed\nTransfer-Encoding: chunked";

	if (errorcode == 500)
		SetupErrorPage("500", "Internal Server Error");
	if (errorcode == 503)
		SetupErrorPage("503", "Unavailable");
	setDate();
	BuildResponseHeader();
}

void	Request::GenerateUploadResponse()
{
	contenttype = "text/plain";
	responsebody = "File was uploaded succesfully";
	contentlength = responsebody.size();
	additionalinfo.clear();
	setDate();
	BuildResponseHeader();
}

void Request::GenerateDeleteResponse()
{
	statuscode = "204 No Content";
	contenttype = "text/plain";
	additionalinfo.clear();
	responsebody = "The File was successfully deleted.";
	contentlength = responsebody.size();
	setDate();
	BuildResponseHeader();
}

void	Request::AssignLocation()
{
	for (std::map<std::string, std::map<std::string, std::string> >::iterator \
		it = config.locations.begin(); it != config.locations.end(); it++)
	{
		// std::cout << it->first << std::endl; 
		if (int pos = requestHeader["location:"].find(it->first) != std::string::npos)
		{
			this->clientpath = it->first;
			this->path = it->second["root:"];
			if (requestHeader["location:"] == it->first)
				this->path += it->second["index:"];
			else
				this->path += requestHeader["location:"].substr(1);
			if (it->second["autoindex:"] == "on")
				autoindex = true;
		}
	}
	// std::cout << "CORRECT PATH: " << path << std::endl;
}

bool	Request::CheckMethod()
{
	if (config.getLocation(clientpath, "allowed_methods:").find(requestHeader["method:"]) \
		!= std::string::npos)
		{
			this->method = requestHeader["method:"];
			return true;
		}
	SetupErrorPage("405", "Method Not Allowed");
	return false;
}

bool	Request::CheckPermissions()
{
	if (access(path.c_str(), W_OK) == -1)
	{
		SetupErrorPage("403", "Forbidden");
		return false;
	}
	if (requestHeader["Content-Type:"] != "multipart/form-data" && access(path.c_str(), X_OK) == -1)
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
	error = true;
	if (status == "413" || status == "500" || status == "503")
		cutoffClient = true;
	this->ObtainFileLength();
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
	time_t currentTime = time(nullptr);
    tm* timeinfo = gmtime(&currentTime);
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    
    std::string conv(buffer);
	this->date = "Date: " + conv;
	// std::cout << date << std::endl;
}

void	Request::SetResponseContentType()
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

	responsebody = autoidx.str();
	additionalinfo = "";
	path = "AUTOINDEX";
	contenttype = "text/html";
	contentlength = responsebody.size();
	// std::cout << autoindexbody << std::endl;
}

