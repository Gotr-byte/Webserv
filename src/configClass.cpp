#include "../includes/configClass.hpp"

/* CONSTRUCTOR SETS CONFIG FILE CONFIGURATIONS TO THE OBJECT AND THEN
IF PROPERTIES ARE MISSING, DEFAULT CONFIGURATIONS WILL BE ADDED */

configClass::configClass(std::string path, int socket_no)
{
	this->setConfProps(path, socket_no);
	this->setDefaultProps();
	if (!this->setLocations(path, socket_no))
		this->setDefaultLocation();
}

configClass::~configClass()
{
}

void configClass::setDefaultProps()
{
	this->properties.insert(std::make_pair("port", "1896"));
	this->properties.insert(std::make_pair("host", "localhost"));
	this->properties.insert(std::make_pair("server_name", "default_server"));
	this->properties.insert(std::make_pair("error_page", "./www/errors/"));
	this->properties.insert(std::make_pair("limit_body_size", "1000"));
	this->properties.insert(std::make_pair("allowed_methods", "GET"));
}

void configClass::setDefaultLocation()
{
	std::map<std::string, std::string> l_props;

	l_props.insert(std::make_pair("root", "./www/html/"));
	l_props.insert(std::make_pair("index", "index.html"));
	l_props.insert(std::make_pair("redirect", "https://google.com/"));
	l_props.insert(std::make_pair("allowed_methods", "POST, GET"));
	l_props.insert(std::make_pair("autoindex", "on"));

	this->locations.insert(std::make_pair("/", l_props));
}

void configClass::setConfProps(std::string path, int socket_no)
{
	std::fstream config;
	std::string line;
	std::string key;
	std::string value;

	config.open(path.c_str(), std::fstream::in);

	this->accessServerBlock(config, socket_no);
	getline(config, line);
	while ((line != "</server>") &&
		   (line.find("<location>") == std::string::npos))
	{
		if (line.find("listen:") != std::string::npos)
		{
			this->properties.insert(std::make_pair("host:",
												   line.substr(line.find(":") + 1, line.find(":") - 1)));
			value = line.substr(line.rfind(":") + 1);
			this->removeWhitespaces(value);

			this->properties.insert(std::make_pair("port:", value));
		}
		else if (line.find(":") != std::string::npos)
		{
			key = line.substr(0, line.find(":") + 1);
			value = line.substr(line.find(":") + 1);

			this->removeWhitespaces(key);
			this->removeWhitespaces(value);

			this->properties.insert(std::make_pair(key, value));
		}
		getline(config, line);
	}
	config.close();
	std::map<std::string, std::string>::iterator it1;
	int i = 0;

	// std::cout << "PROPERTIES:\n";
	// for (it1 = properties.begin(); it1 != properties.end(); it1++)
	// {
	// 	std::cout << "value: " << it1->first << " key: " << it1->second << std::endl;
	// }
	// i++;
}

void configClass::accessServerBlock(std::fstream &config, int socket_no)
{
	std::string tmp;

	while (0 < socket_no)
	{
		getline(config, tmp);
		if (tmp == "</server>")
			socket_no--;
	}
}

bool configClass::setLocations(std::string path, int socket_no)
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
					exit(1);
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

				block.insert(std::make_pair(key, value));
			}
			getline(config, line);
		}
		checkLocationBlock(block, dir);
		getline(config, line);
	}
	// std::map<std::string, std::map<std::string, std::string> >::iterator it1;
	// std::map<std::string, std::string>::iterator it2;
	// int i = 0;

	// std::cout << "LOCATIONS:\n";
	// for (it1 = locations.begin(); it1 != locations.end(); it1++)
	// {
	// 	std::cout << "Block " << i << " with location: " << it1->first << std::endl;
	// 	for (it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
	// 	{
	// 		std::cout << "value: " << it2->first << " key: " << it2->second << std::endl;
	// 	}
	// 	i++;
	// }
	return true;
}

void configClass::checkLocationBlock(std::map<std::string, std::string> & block, std::string dir)
{
		if (dir == "")
		{
			std::cout << "LocationBlock: Doesnt contain location\n";
			exit(1);
		}
		if (block.find("root:") == block.end())
		{
			std::cout << "LocationBlock: No root directory set\n";
			exit(1);
		}
		if (block.find("allowed_methods:") == block.end())
			block["allowed_methods:"] = this->properties["allowed_methods:"];
		if (block.find("autoindex:") == block.end())
			block["autoindex:"] = "on";
		if (!this->locations.insert(std::make_pair(dir, block)).second)
		{
			std::cout << "LocationBlock: Location duplicate detected\n";
			exit(1);
		}
}

std::string	configClass::getConfProps(std::string key)
{
	return this->properties[key];
}

std::string	configClass::getLocation(std::string location, std::string key)
{
	return this->locations[location][key];
}


void configClass::removeWhitespaces(std::string &string)
{
	string.erase(0, string.find_first_not_of(" \t"));
	string.erase(string.find_last_not_of(" \t") + 1);
}

