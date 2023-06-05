# include "../includes/RequestProcessor.hpp"
// # include 


RequestProcessor::RequestProcessor(std::map<std::string, std::string> req, ServerConfig	conf) \
									: request(req), config(conf), isdirectory(false), \
										autoindex(false), additionalinfo("")
{
	this->protocoll = "HTTP/1.1";
	this->AssignLocation();
	if (this->CheckMethod() && this->CheckPath())
	{

	}
	this->setDate();
}

RequestProcessor::~RequestProcessor()
{}

void	RequestProcessor::AssignLocation()
{
	for (std::map<std::string, std::map<std::string, std::string> >::iterator \
		it = config.locations.begin(); it != config.locations.end(); it++)
	{
		std::cout << it->first << std::endl; 
		if (int pos = request["location:"].find(it->first) != std::string::npos)
		{
			this->clientlocation = it->first;
			this->path = it->second["root:"];
			if (request["location:"] == it->first)
				this->path += it->second["index:"];
			else
				this->path += request["location:"].substr(1);
			if (it->second["autoindex:"] == "on")
				autoindex = true;
		}
	}
	std::cout << "CORRECT PATH: " << path << std::endl;
}

bool	RequestProcessor::CheckMethod()
{
	if (config.getLocation(clientlocation, "allowed_methods:").find(request["method:"]) \
		!= std::string::npos)
		return true;
	SetupErrorPage("405", "Method Not Allowed");
	additionalinfo = "Allow: " + config.getLocation(clientlocation, "allowed_methods:");
	return false;
}

bool	RequestProcessor::CheckPath()
{
	if (CheckExistance() && IsFile())
		return true;
	else if (IsDirectory() && !autoindex)
	{
		SetupErrorPage("403", "Forbidden");
		return false;
	}
	else if (IsDirectory() && autoindex)
	{
		(void) isdirectory;
		// SETUP AUTOINDEX PAGE
	}
	return false;
}

void	RequestProcessor::SetupErrorPage(std::string status, std::string issue)
{
	path = "../error_pages/" + status + ".html";
	statuscode = status + " " + issue;
	contenttype = "text/html";
	method = "GET";
	ObtainFileLength();
	// std::cout << content << std::endl;
	// std::cout << path << std::endl;
	// std::cout << statuscode << std::endl;
	// std::cout << contenttype << std::endl;
	// std::cout << method << std::endl;
}

void	RequestProcessor::ObtainFileLength()
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

bool	RequestProcessor::CheckExistance()
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

bool	RequestProcessor::IsFile()
{
	FILE *file = fopen(path.c_str(), "r");
	if (file)
	{
		std::fclose(file);
		return true;
	}
	return false;
}

bool	RequestProcessor::IsDirectory()
{
	DIR *dir = opendir(path.c_str());
	if (!dir)
	{
		SetupErrorPage("404", "Not Found");
		return false;
	}
	closedir(dir);
	isdirectory = true;
	return true;

}

void	RequestProcessor::setDate()
{
	std::time_t currentTime = std::time(nullptr);
    std::tm* timeinfo = std::gmtime(&currentTime);
    
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    
    std::string conv(buffer);
	this->date = "Date: " + conv;
	// std::cout << date << std::endl;
}