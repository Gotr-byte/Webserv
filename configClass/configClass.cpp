#include "configClass.hpp"

/* CONSTRUCTOR SETS CONFIG FILE CONFIGURATIONS TO THE OBJECT AND THEN 
IF PROPERTIES ARE MISSING, DEFAULT CONFIGURATIONS WILL BE ADDED */

configClass::configClass(std::string path, int socket_no)
{
	this->setConfProps(path, socket_no);
	this->setDefaultProps();
	if (!this->setLocations())
		this->setDefaultLocation();
}

configClass::~configClass()
{}

void	configClass::setDefaultProps()
{
	this->properties.insert(std::make_pair("port", "1896"));
	this->properties.insert(std::make_pair("host", "localhost"));
	this->properties.insert(std::make_pair("server_name", "default_server"));
	this->properties.insert(std::make_pair("error_page", "./www/errors/"));
	this->properties.insert(std::make_pair("limit_body_size", "1000"));
	this->properties.insert(std::make_pair("allowed_methods", "GET"));
}

void	configClass::setDefaultLocation()
{
	std::map<std::string, std::string>	l_props;

	l_props["root"] = "./www/html/";
	l_props["index"] = "index.html";
	l_props["redirect"] = "https://google.com/";
	l_props["allowed_methods"] = "POST, GET";
	l_props["autoindex"] = "on";
	
	this->locations.insert(std::make_pair("/", l_props));
}

void	configClass::setConfProps(std::string path, int socket_no)
{
	std::fstream	config(path);
	std::string		line;
	std::string		key;
	std::string		value;

	while (0 < socket_no)
	{
		getline(config, line);
		if (line == "</server>")
			socket_no--;
	}
	getline(config, line);
	while ((line != "</server>") && \
			(line.find("<location>") == std::string::npos))
	{
		if (line.find("listen") != std::string::npos)
		{
			key = line.substr(0, line.find(":"));
			value = line.substr(line.find(":") + 1);
		}
		else if (line.find(":") != std::string::npos)
			std::cout << "test";
		getline(config, line);
	}
	config.close();
}


bool	configClass::setLocations()
{
	return true;
}