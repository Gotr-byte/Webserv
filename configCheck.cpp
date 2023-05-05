#include <iostream>
#include <fcntl.h>
#include <string>
#include <fstream>
#include <stdexcept>

void	checkLocationBlock(std::fstream &istream, std::string &line)
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

void	checkServerBlock(std::fstream	&istream, std::string	&line)
{
	int ret = 0;
	if (line != "<server>")
		throw std::logic_error("ServerBlock has to start with <server>");
	while (line != "</server>")
	{
		if (istream.eof())
			throw std::logic_error("Unexpected end of ServerBlock");
		getline(istream, line);
			std::cout << line << std::endl;
		if (line.find("<location>") != std::string::npos){
			checkLocationBlock(istream, line);
		}
		else if (line.find("</location>") != std::string::npos)
			throw std::logic_error("Incorrect LocationBlock format");
		if (line.find("<server>") != std::string::npos)
			throw std::logic_error("ServerBlock cannot contain another ServerBlock start <server>");
	}
}

int	checkConfig(std::string PATH)
{
	std::fstream	istream;
	std::string		line;
	int				server_counter;

	istream.open(PATH, std::fstream::in);

	while (!getline(istream, line).eof())
	{
		if (!line.empty())
		{
			checkServerBlock(istream, line);
			server_counter++;
		}
	}
	std::cout << server_counter << std::endl;
	return server_counter;
}

int main()
{
	checkConfig("./sample.config");
	return 1;
}
