#include "../includes/Response.hpp"

Response::Response()
{
	protocoll = "HTTP/1.1";
	contenttype = "application/octet-stream";
	statuscode = "200 OK";
	is_chunked = false;
}

void	Response::CreateResponse(ServerConfig	conf)
{
	contenttype = "application/octet-stream";
	statuscode = "200 OK";
	protocoll = "HTTP/1.1";
}

void	Response::GenerateClientErrorResponse(std::string status, std::string issue)
{
	protocoll = "HTTP/1.1", \
	additionalinfo = "Connection: closed\nTransfer-Encoding: chunked";
	SetupErrorPage(status, issue);
	setDate();
	BuildResponseHeader();
}

void	Response::GenerateServerErrorResponse(int errorcode, ServerConfig	conf)
{
	protocoll = "HTTP/1.1", \
	additionalinfo = "Connection: closed\nTransfer-Encoding: chunked";

	if (errorcode == 500)
		SetupErrorPage("500", "Internal Server Error");
	if (errorcode == 503)
		SetupErrorPage("503", "Unavailable");
	setDate();
	BuildResponseHeader();
}

void	Response::GenerateUploadResponse()
{
	contenttype = "text/plain";
	body = "File was uploaded succesfully\r\n\r\n";
	contentlength = body.size();
	additionalinfo.clear();
	BuildResponseHeader();
}

void	Response::generate_cgi_response(std::string path_to_HTML)
{
	ObtainFileLength(path_to_HTML);
	contenttype = "text/html";
	setDate();
	BuildResponseHeader();
}

// void Response::setup_cgi_page(std::string file_to_upload){

// }

void Response::GenerateDeleteResponse()
{
	statuscode = "204 No Content";
	contenttype = "text/plain";
	additionalinfo.clear();
	body = "The File was successfully deleted.\r\n\r\n";
	contentlength = body.size();
	setDate();
	BuildResponseHeader();
}

void	Response::SetupErrorPage(std::string status, std::string issue)
{
	statuscode = status + " " + issue;
	contenttype = "text/html";
	this->ObtainFileLength(error_path);
	BuildResponseHeader();
	// std::cout << path << std::endl;
	// std::cout << statuscode << std::endl;
	// std::cout << contenttype << std::endl;
	// std::cout << method << std::endl;
}

void	Response::ObtainFileLength(std::string path)
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

void	Response::SetResponseContentType(std::string path)
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

void	Response::BuildResponseHeader()
{
	std::ostringstream tmp;

	if (BUF_SIZE < contentlength)
	{
		additionalinfo = "Transfer-Encoding: chunked";
		is_chunked = true;
	}

	this->setDate();
	tmp << protocoll << " " << statuscode << "\r\n";
	tmp << "Server: " << server_name << "\r\n";
	tmp << date << "\r\n";
	tmp << "Content-Type: " << contenttype << "\r\n";
	tmp << "Content-Length: " << contentlength << "\r\n";
	if (!additionalinfo.empty())
		tmp << additionalinfo << "\r\n";
	tmp << "\r\n";

	header = tmp.str();
	// std::cout << header << std::endl;
}

void	Response::CreateAutoindex(std::string path)
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
	autoidx << "\r\n\r\n";

	body = autoidx.str();
	additionalinfo = "";
	contenttype = "text/html";
	contentlength = body.size();
	// std::cout << autoindexbody << std::endl;
}

