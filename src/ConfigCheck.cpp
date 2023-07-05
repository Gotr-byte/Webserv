#include "../includes/ConfigCheck.hpp"

ConfigCheck::ConfigCheck() : server_counter(0)
{}

ConfigCheck::~ConfigCheck()
{}

int	ConfigCheck::checkConfig(std::string path)
{
	std::fstream	config;
	std::string		line;

	config.open(path.c_str(), std::fstream::in);

	while (!getline(config, line).eof())
	{
		if (!line.empty())
		{
			checkSocketBlock(config, line);
			server_counter++;
		}
	}
	config.close();
	return server_counter;
}

void	ConfigCheck::checkSocketBlock(std::fstream &config, std::string &line)
{
	if (line != "<socket>")
		throw std::logic_error("Config file: SocketBlock has to start with <socket>");
	while (line != "</socket>")
	{
		if (line.empty())
			getline(config, line);
		if (config.eof())
			throw std::logic_error("Config file: Unexpected end of SocketBlock");
		getline(config, line);
		if (line.find("<server>") != std::string::npos)
			checkServerBlock(config, line);
		else if (line.find("</server>") != std::string::npos)
			throw std::logic_error("Config file: Incorrect ServerBlock format");
		else if (line.find("<location>") != std::string::npos)
			throw std::logic_error("Config file: Incorrect LocationBlock format");
		else if (line.find("</location>") != std::string::npos)
			throw std::logic_error("Config file: Incorrect LocationBlock format");
		if (line.find("<socket>") != std::string::npos)
			throw std::logic_error("Config file: SocketBlock cannot contain another SocketBlock start <socket>");
	}
}

void	ConfigCheck::checkServerBlock(std::fstream	&config, std::string	&line)
{
	while (line.find("</server>") == std::string::npos)
	{
		if (line.empty())
			getline(config, line);
		if (config.eof())
			throw std::logic_error("Config file: Unexpected end of ServerBlock");
		getline(config, line);
		if (line.find("<location>") != std::string::npos)
			checkLocationBlock(config, line);
		else if (line.find("<socket>") != std::string::npos)
			throw std::logic_error("Config file: ServerBlock cannot contain another SocketBlock start <server>");
		else if (line.find("</location>") != std::string::npos)
			throw std::logic_error("Config file: Incorrect LocationBlock format");
		else if (line.find("<server>") != std::string::npos)
			throw std::logic_error("Config file: ServerBlock cannot contain another ServerBlock start <server>");
	}
}

void	ConfigCheck::checkLocationBlock(std::fstream &config, std::string &line)
{
	while (line.find("</location>") == std::string::npos)
	{
		if (line.empty())
			getline(config, line);
		if (config.eof())
			throw std::logic_error("Config file: Unexpected end of LocationBlock");
		getline(config, line);
		if (line.find("<server>") != std::string::npos)
			throw std::logic_error("Config file: No ServerBlock allowed in LocationBlock");
		else if (line.find("<socket>") != std::string::npos)
			throw std::logic_error("Config file: No SocketBlock allowed in LocationBlock");
		else if (line.find("<location>") != std::string::npos)
			throw std::logic_error("Config file: LocationBlock cannot contain another LocationBlock start <location>");
	}
}
