#include "../includes/ServerConfig.hpp"

/* CONSTRUCTOR SETS CONFIG FILE CONFIGURATIONS TO THE OBJECT AND THEN
IF PROPERTIES ARE MISSING, DEFAULT CONFIGURATIONS WILL BE ADDED */

SocketConfig::SocketConfig(std::string path, int socket_no)
{
	this->setServerConfigs(path, socket_no);
}

void	SocketConfig::setServerConfigs(std::string path, int socket_no)
{
	std::fstream config;
	std::string line;
	std::string key;
	std::string value;

	config.open(path.c_str(), std::fstream::in);

	this->accessSocketBlock(config, socket_no);
	getline(config, line);
	while ((line != "</socket>") &&
		   (line.find("<server>") == std::string::npos))
	{
		if (line.find(":") != std::string::npos)
		{
			key = line.substr(0, line.find(":") + 1);
			value = line.substr(line.find(":") + 1);

			this->removeWhitespaces(key);
			this->removeWhitespaces(value);

			if (key == "listen:")
			{
				this->host = value.substr(0, value.find(":"));
				this->port = value.substr(value.find(":") + 1);
			}
			this->properties[key] = value;
		}
		getline(config, line);
	}
	config.close();
}

void SocketConfig::accessSocketBlock(std::fstream &config, int socket_no)
{
	std::string tmp;

	while (0 < socket_no)
	{
		getline(config, tmp);
		if (tmp == "</socket>")
			socket_no--;
	}
}

SocketConfig::ServerConfig::ServerConfig(std::string path, int socket_no)
{

	this->setConfProps(path, socket_no);
	this->setDefaultProps();
	if (!this->setLocations(path, socket_no))
		this->setDefaultLocation();
}

SocketConfig::ServerConfig::~ServerConfig()
{}

void SocketConfig::ServerConfig::setDefaultProps()
{
	this->properties.insert(std::make_pair("port:", "1896"));
	this->properties.insert(std::make_pair("host:", "localhost"));
	this->properties.insert(std::make_pair("server_name:", "default_server"));
	this->properties.insert(std::make_pair("error_page:", "www/error_pages/"));
	this->properties.insert(std::make_pair("limit_body_size:", "100"));
	this->properties.insert(std::make_pair("allowed_methods:", "GET"));
}

void SocketConfig::ServerConfig::setDefaultLocation()
{
	std::map<std::string, std::string> l_props;

	l_props.insert(std::make_pair("root:", "www/HTML/"));
	l_props.insert(std::make_pair("index:", "index.html"));
	l_props.insert(std::make_pair("redirect:", "https://google.com/"));
	l_props.insert(std::make_pair("allowed_methods:", "POST, GET"));
	l_props.insert(std::make_pair("cgi_path:", "www/HTML/cgi-bin/"));
	l_props.insert(std::make_pair("cgi_ext:", ".py"));
	l_props.insert(std::make_pair("autoindex:", "on"));

	this->locations.insert(std::make_pair("/", l_props));
}

void SocketConfig::ServerConfig::setConfProps(std::string path, int socket_no)
{
	std::fstream config;
	std::string line;
	std::string key;
	std::string value;

	config.open(path.c_str(), std::fstream::in);

	SocketConfig::accessSocketBlock(config, socket_no);
	getline(config, line);
	while ((line != "</server>") &&
		   (line.find("<location>") == std::string::npos))
	{
		if (line.find(":") != std::string::npos)
		{
			key = line.substr(0, line.find(":") + 1);
			value = line.substr(line.find(":") + 1);

			SocketConfig::removeWhitespaces(key);
			SocketConfig::removeWhitespaces(value);

			if (key == "listen:")
			{
				this->host = value.substr(0, value.find(":"));
				this->port = value.substr(value.find(":") + 1);
			}
			this->properties[key] = value;
		}
		getline(config, line);
	}
	config.close();
}

void SocketConfig::ServerConfig::accessServerBlock(std::fstream &config, int socket_no)
{
	std::string tmp;

	while (0 < socket_no)
	{
		getline(config, tmp);
		if (tmp == "</server>")
			socket_no--;
	}
}

bool SocketConfig::ServerConfig::setLocations(std::string path, int socket_no)
{
	std::fstream config;
	std::string line;
	std::string dir;
	std::string key;
	std::string value;

	config.open(path.c_str(), std::fstream::in);

	this->accessServerBlock(config, socket_no);
	getline(config, line);
	while (line.find("<location>") == std::string::npos)
	{
		if (line == "</server>")
		{
			std::cout << "No LocationBlock exisiting, using default location settings\n";
			return false;
		}
		getline(config, line);
	}
	while (line.find("</server>") == std::string::npos)
	{
		std::map<std::string, std::string> block;

		getline(config, line);
		dir = "";
		while (line.find("</location>") == std::string::npos)
		{
			if (line.find("location:") != std::string::npos)
			{
				if (dir != "")
				{
					std::cout << "LocationBlock: Only one location per LocationBlock allowed\n";
					exit(EXIT_FAILURE);
				}
				dir = line.substr(line.rfind(":") + 1);
				this->removeWhitespaces(dir);
			}
			else if (line.find(":") != std::string::npos)
			{
				key = line.substr(0, line.find(":") + 1);
				value = line.substr(line.find(":") + 1);

				this->removeWhitespaces(key);
				this->removeWhitespaces(value);

				block[key] = value;
			}
			getline(config, line);
		}
		checkLocationBlock(block, dir);
		getline(config, line);
	}
	std::map<std::string, std::map<std::string, std::string> >::iterator it1;
	std::map<std::string, std::string>::iterator it2;
	return true;
}

void SocketConfig::ServerConfig::checkLocationBlock(std::map<std::string, std::string> & block, std::string dir)
{
		if (dir == "")
		{
			std::cout << "LocationBlock: Doesnt contain location\n";
			exit(EXIT_FAILURE);
		}
		if (block.find("root:") == block.end() && block.find("redirect:") == block.end() )
		{
			std::cout << "LocationBlock: No root directory or redirect set\n";
			exit(EXIT_FAILURE);
		}
		if (block.find("root:") != block.end() && block.find("redirect:") != block.end() )
		{
			std::cout << "LocationBlock: Either relocation or redirection is possible, not both in one location\n";
			exit(EXIT_FAILURE);
		}
		if (block.find("allowed_methods:") == block.end())
			block["allowed_methods:"] = this->properties.at("allowed_methods:");
		if (block.find("redirect:") == block.end() && \
			(block.at("allowed_methods:").find("GET") != std::string::npos || block.at("allowed_methods:").find("POST") != std::string::npos))
		{
			if (block.find("cgi_ext:") == block.end() && block.find("cgi_path:") == block.end())
			{
				std::cout << "LocationBlock: POST & GET method require cgi_path and cgi_ext\n";
				exit(EXIT_FAILURE);
			}
			else if (block.find("cgi_ext:") == block.end() && block.at("cgi_ext") != ".py")
			{
				std::cout << "LocationBlock: This webserver just support .py cgi scripts\n";
				exit(EXIT_FAILURE);
			}
		}
		if (block.find("autoindex:") == block.end())
			block["autoindex:"] = "on";
		if (!this->locations.insert(std::make_pair(dir, block)).second)
		{
			std::cout << "LocationBlock: Location duplicate detected\n";
			exit(EXIT_FAILURE);
		}
}

std::string	SocketConfig::ServerConfig::getConfProps(std::string key)
{
	return this->properties[key];
}

std::string	SocketConfig::ServerConfig::getLocation(std::string location, std::string key)
{
	return this->locations[location][key];
}

std::map<std::string, \
	std::map<std::string, std::string> > SocketConfig::ServerConfig::getLocations()
{
	return this->locations;
}

void SocketConfig::ServerConfig::removeWhitespaces(std::string &string)
{
	string.erase(0, string.find_first_not_of(" \t"));
	string.erase(string.find_last_not_of(" \t") + 1);
}

