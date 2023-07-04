#include "../includes/Response.hpp"

Response::Response()
{
	protocoll = "HTTP/1.1";
	content_type = "application/octet-stream";
	status_code = "200 OK";
	is_chunked = false;
}

void	Response::generateRedirectionResponse(std::string URL)
{
	status_code = "301 Moved Permanently";
	additional_info = "Location: " + URL;
	buildResponseHeader();
}

void	Response::generateErrorResponse(std::string status, std::string issue)
{
	additional_info = "Connection: closed\nTransfer-Encoding: chunked";
	setupErrorPage(status, issue);
	buildResponseHeader();
}

void	Response::generateUploadResponse(std::string file_path)
{
	status_code = "201 Created";
	content_type = "text/plain";
	body = "File was uploaded succesfully";
	content_length = body.size();
	additional_info = "Location: " + file_path;
	buildResponseHeader();
}

void	Response::generateCgiResponse(std::string path)
{
	obtainFileLength(path);
	content_type = "text/html";
	buildResponseHeader();
}

void Response::generateDeleteResponse()
{
	status_code = "204 No Content";
	content_type = "text/plain";
	additional_info.clear();
	content_length = body.size();
	setDate();
	buildResponseHeader();
}

void	Response::setupErrorPage(std::string status, std::string issue)
{
	status_code = status + " " + issue;
	content_type = "text/html";
	this->obtainFileLength(error_path);
	buildResponseHeader();
}

void	Response::obtainFileLength(std::string path)
{
	FILE* file = fopen(path.c_str(), "r");
	if (file == nullptr)	
	{
		std::cerr << ("response: Error opening file");
		exit(EXIT_FAILURE);
	}
	std::fseek(file, 0, SEEK_END);
	content_length = std::ftell(file);
	std::fclose(file);
}

void	Response::setDate()
{
	time_t currentTime = time(nullptr);
    tm* timeinfo = gmtime(&currentTime);
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    
    std::string conv(buffer);
	this->date = "Date: " + conv;
	// std::cout << date << std::endl;
}

void	Response::setResponseContentType(std::string path)
{
	std::string suffix = path.substr(path.rfind(".") + 1);

	if (path.find("file/") != std::string::npos)
		content_type = "application/octet-stream";
	else
	{
		if (suffix == "html")
			content_type = "text/html";
		else if (suffix == "css")
			content_type = "text/css";
		else if (suffix == "txt")
			content_type = "text/plain";
		else if (suffix == "ico")
			content_type = "image/x-icon";
		else if (suffix == "jpg" || suffix == "jpeg")
			content_type = "image/jpeg";
		else if (suffix == "png")
			content_type = "image/png";
		else if (suffix == "gif")
			content_type = "image/gif";
		else if (suffix == "pdf")
			content_type = "application/pdf";
		else if (suffix == "mp3")
			content_type = "audio/mpeg";
		else if (suffix == "mp4")
			content_type = "audio/mpeg";
		else if (suffix == "avi")
			content_type = "video/x-msvideo";
		else
			content_type = "application/octet-stream";
	}
}

void	Response::buildResponseHeader()
{
	std::ostringstream tmp;

	if (BUF_SIZE < content_length)
	{
		additional_info = "Transfer-Encoding: chunked";
		is_chunked = true;
	}

	this->setDate();
	tmp << protocoll << " " << status_code << "\r\n";
	tmp << "Server: " << server_name << "\r\n";
	tmp << date << "\r\n";
	if (status_code != "204 No Content" || status_code != "301 Moved Permanently")
	{
		tmp << "Content-Type: " << content_type << "\r\n";
		tmp << "Content-Length: " << content_length << "\r\n";
	}
	if (!additional_info.empty())
		tmp << additional_info << "\r\n";
	tmp << "\r\n";

	header = tmp.str();
}

void	Response::createAutoindex(std::string path)
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

	struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
	{
        // Skip "." and ".." entries
        if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
            continue;
        }
        
        std::string itemName = entry->d_name;
        std::string itemPath = path + "/" + itemName;
        
        // Check if the entry is a file or directory
        bool isDir = (entry->d_type == DT_DIR);
        
        // generate the appropriate HTML entry
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
    autoidx << "</html>";

	body = autoidx.str();
	additional_info = "";
	content_type = "text/html";
	content_length = body.size();
	// std::cout << autoindexbody << std::endl;
}

