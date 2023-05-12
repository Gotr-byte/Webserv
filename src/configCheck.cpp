// #include <iostream>
// #include <fcntl.h>
// #include <string>
// #include <fstream>
// #include <stdexcept>
#include "../includes/ConfigCheck.hpp"

static void	checkLocationBlock(std::fstream &config, std::string &line)
{
	while (line.find("</location>") == std::string::npos)
	{
		if (config.eof())
			throw std::logic_error("Unexpected end of LocationBlock");
		getline(config, line);
		if (line.find("server>") != std::string::npos)
			throw std::logic_error("No ServerBlock allowed in LocationBlock");
		else if (line.find("<location>") != std::string::npos)
			throw std::logic_error("LocationBlock cannot contain another location block start <location>");
	}
}

static void	checkServerBlock(std::fstream	&config, std::string	&line)
{
	if (line != "<server>")
		throw std::logic_error("ServerBlock has to start with <server>");
	while (line != "</server>")
	{
		if (config.eof())
			throw std::logic_error("Unexpected end of ServerBlock");
		getline(config, line);
		if (line.find("<location>") != std::string::npos)
			checkLocationBlock(config, line);
		else if (line.find("</location>") != std::string::npos)
			throw std::logic_error("Incorrect LocationBlock format");
		if (line.find("<server>") != std::string::npos)
			throw std::logic_error("ServerBlock cannot contain another ServerBlock start <server>");
	}
}

int	ConfigCheck::checkConfig(std::string path)
{
	std::fstream	config;
	std::string		line;
	int				server_counter = 0;

	config.open(path.c_str(), std::fstream::in);

	while (!getline(config, line).eof())
	{
		if (!line.empty())
		{
			checkServerBlock(config, line);
			server_counter++;
		}
	}
	config.close();
	return server_counter;
}

