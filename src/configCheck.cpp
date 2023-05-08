// #include <iostream>
// #include <fcntl.h>
// #include <string>
// #include <fstream>
// #include <stdexcept>
#include "../includes/ConfigCheck.hpp"

static void	checkLocationBlock(std::fstream &istream, std::string &line)
{
	while (line.find("</location>") == std::string::npos)
	{
		if (istream.eof())
			throw std::logic_error("Unexpected end of LocationBlock");
		getline(istream, line);
		if (line.find("server>") != std::string::npos)
			throw std::logic_error("No ServerBlock allowed in LocationBlock");
		else if (line.find("<location>") != std::string::npos)
			throw std::logic_error("LocationBlock cannot contain another location block start <location>");
	}
}

static void	checkServerBlock(std::fstream	&istream, std::string	&line)
{
	if (line != "<server>")
		throw std::logic_error("ServerBlock has to start with <server>");
	while (line != "</server>")
	{
		if (istream.eof())
			throw std::logic_error("Unexpected end of ServerBlock");
		getline(istream, line);
		if (line.find("<location>") != std::string::npos)
			checkLocationBlock(istream, line);
		else if (line.find("</location>") != std::string::npos)
			throw std::logic_error("Incorrect LocationBlock format");
		if (line.find("<server>") != std::string::npos)
			throw std::logic_error("ServerBlock cannot contain another ServerBlock start <server>");
	}
}

int	ConfigCheck::checkConfig(std::string path)
{
	std::fstream	istream;
	std::string		line;
	int				server_counter = 0;

	istream.open(path, std::fstream::in);

	while (!getline(istream, line).eof())
	{
		if (!line.empty())
		{
			checkServerBlock(istream, line);
			server_counter++;
		}
	}
	return server_counter;
}

