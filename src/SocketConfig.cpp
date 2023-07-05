#include "../includes/SocketConfig.hpp"

/* CONSTRUCTOR SETS CONFIG FILE CONFIGURATIONS TO THE OBJECT AND THEN
IF PROPERTIES ARE MISSING, DEFAULT CONFIGURATIONS WILL BE ADDED */

SocketConfig::SocketConfig(std::string path, int socket_no) : servers_on_port(-1)
{
	this->setupPortHostServerNo(path, socket_no);
	if (servers_on_port < 0)
	{
		std::cerr << "No Server defined at " << host << ":" << port << std::endl;
		exit(EXIT_FAILURE);
	}
	for (int server_no = 0; server_no <= servers_on_port; server_no++)
		this->setServerConfigs(path, socket_no, server_no);
}

SocketConfig::~SocketConfig()
{}

void	SocketConfig::setupPortHostServerNo(std::string path, int socket_no)
{
	std::fstream config;
	std::string value;
	std::string line;

	config.open(path.c_str(), std::fstream::in);

	this->accessSocketBlock(config, socket_no);
	getline(config, line);
	while (line != "</socket>")
	{
		if (line.find("<server>") != std::string::npos)
			servers_on_port++;
		else if (line.find("listen:") != std::string::npos)
		{
			value = line.substr(line.find(":") + 1);
			this->removeWhitespaces(value);

			this->host = value.substr(0, value.find(":"));
			this->port = value.substr(value.find(":") + 1);
		}
		getline(config, line);
	}
	if (this->host.empty() || this->port.empty())
	{
		std::cerr << "Config file: invalid or missing listen: entry\n";
		exit(EXIT_FAILURE);
	}
	config.close();
}

void	SocketConfig::setServerConfigs(std::string path, int socket_no, int server_no)
{
	std::fstream config;
	std::string line;
	std::string server_name = "";
	std::string value;

	config.open(path.c_str(), std::fstream::in);

	this->accessSocketBlock(config, socket_no);
	this->accessServerBlock(config, server_no);
	getline(config, line);
	while (line.find("</server>") == std::string::npos)
	{
		if (line.find("server_name:") != std::string::npos)
		{
			server_name = line.substr(line.find(":") + 1);
			this->removeWhitespaces(server_name);
			this->servers.insert(std::make_pair(server_name, ServerConfig(path, socket_no, server_no)));
		}
		getline(config, line);
	}
	if (server_name.empty())
	{
		std::cerr << "Config File: missing server_name\n";
		exit(EXIT_FAILURE);
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

void	SocketConfig::accessServerBlock(std::fstream &config, int server_no)
{
	std::string tmp;

	while (0 < server_no)
	{
		getline(config, tmp);
		if (tmp.find("</server>") != std::string::npos)
			server_no--;
	}
}

void	SocketConfig::removeWhitespaces(std::string &string)
{
	string.erase(0, string.find_first_not_of(" \t"));
	string.erase(string.find_last_not_of(" \t") + 1);
}

SocketConfig::ServerConfig::ServerConfig(std::string path, int socket_no, int server_no)
{
	this->setConfProps(path, socket_no, server_no);
	this->setDefaultProps();
	if (!this->setLocations(path, socket_no, server_no))
		this->setDefaultLocation();
}

SocketConfig::ServerConfig::ServerConfig()
{}

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

void SocketConfig::ServerConfig::setConfProps(std::string path, int socket_no, int server_no)
{
	std::fstream config;
	std::string line;
	std::string key;
	std::string value;

	config.open(path.c_str(), std::fstream::in);

	SocketConfig::accessSocketBlock(config, socket_no);
	SocketConfig::accessServerBlock(config, server_no);
	getline(config, line);
	while ((line != "</server>") &&
		   (line.find("<location>") == std::string::npos))
	{
		if (line.find(":") != std::string::npos && line.find("listen:") == std::string::npos)
		{
			key = line.substr(0, line.find(":") + 1);
			value = line.substr(line.find(":") + 1);

			SocketConfig::removeWhitespaces(key);
			SocketConfig::removeWhitespaces(value);

			this->properties.insert(std::make_pair(key, value));
		}
		getline(config, line);
	}
	config.close();
}

bool SocketConfig::ServerConfig::setLocations(std::string path, int socket_no, int server_no)
{
	std::fstream config;
	std::string line;
	std::string dir;
	std::string key;
	std::string value;

	config.open(path.c_str(), std::fstream::in);

	SocketConfig::accessSocketBlock(config, socket_no);
	SocketConfig::accessServerBlock(config, server_no);
	getline(config, line);
	while (line.find("<location>") == std::string::npos)
	{
		if (line == "</server>")
		{
			std::cerr << "Config file: No LocationBlock exisiting, using default location settings\n";
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
					std::cerr << "Config file: Only one location per LocationBlock allowed\n";
					exit(EXIT_FAILURE);
				}
				dir = line.substr(line.rfind(":") + 1);
				SocketConfig::removeWhitespaces(dir);
			}
			else if (line.find(":") != std::string::npos)
			{
				key = line.substr(0, line.find(":") + 1);
				value = line.substr(line.find(":") + 1);

				SocketConfig::removeWhitespaces(key);
				SocketConfig::removeWhitespaces(value);

				block.insert(std::make_pair(key, value));
			}
			getline(config, line);
		}
		checkLocationBlock(block, dir);
		getline(config, line);
	}
	return true;
}

void SocketConfig::ServerConfig::checkLocationBlock(std::map<std::string, std::string> & block, std::string dir)
{
		if (dir == "")
		{
			std::cerr << "Config file: Doesnt contain location\n";
			exit(EXIT_FAILURE);
		}
		if (block.find("root:") == block.end() && block.find("redirect:") == block.end() )
		{
			std::cerr << "Config file: No root directory or redirect set\n";
			exit(EXIT_FAILURE);
		}
		if (block.find("root:") != block.end() && block.find("redirect:") != block.end() )
		{
			std::cerr << "Config file: Either relocation or redirection is possible, not both in one location\n";
			exit(EXIT_FAILURE);
		}
		if (block.find("allowed_methods:") == block.end())
			block["allowed_methods:"] = this->properties.at("allowed_methods:");
		if (block.find("redirect:") == block.end() && \
			(block.at("allowed_methods:").find("GET") != std::string::npos || block.at("allowed_methods:").find("POST") != std::string::npos))
		{
			if (block.find("cgi_ext:") == block.end() && block.find("cgi_path:") == block.end())
			{
				std::cerr << "Config file: POST & GET method require cgi_path and cgi_ext\n";
				exit(EXIT_FAILURE);
			}
			else if (block.find("cgi_ext:") == block.end() && block.at("cgi_ext") != ".py")
			{
				std::cerr << "Config file: This webserver just support .py cgi scripts\n";
				exit(EXIT_FAILURE);
			}
		}
		if (block.find("autoindex:") == block.end())
			block["autoindex:"] = "on";
		if (!this->locations.insert(std::make_pair(dir, block)).second)
		{
			std::cerr << "Config file: Location duplicate detected\n";
			exit(EXIT_FAILURE);
		}
}

std::string	SocketConfig::ServerConfig::getConfProps(std::string key)
{
	return this->properties.at(key);
}

std::string	SocketConfig::ServerConfig::getLocation(std::string location, std::string key)
{
	return this->locations.at(location).at(key);
}

std::map<std::string, \
	std::map<std::string, std::string> > SocketConfig::ServerConfig::getLocations()
{
	return this->locations;
}

