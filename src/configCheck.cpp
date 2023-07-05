#include "../includes/ConfigCheck.hpp"

static void	checkLocationBlock(std::fstream &config, std::string &line)
{
	while (line.find("</location>") == std::string::npos)
	{
		if (line.empty())
			getline(config, line);
		if (config.eof())
			throw std::logic_error("Unexpected end of LocationBlock");
		getline(config, line);
		std::cout << "location block: " << line << std::endl;
		if (line.find("<server>") != std::string::npos)
			throw std::logic_error("No ServerBlock allowed in LocationBlock");
		else if (line.find("<socket>") != std::string::npos)
			throw std::logic_error("No SocketBlock allowed in LocationBlock");
		else if (line.find("<location>") != std::string::npos)
			throw std::logic_error("LocationBlock cannot contain another LocationBlock start <location>");
	}
}

static void	checkServerBlock(std::fstream	&config, std::string	&line)
{
	while (line.find("</server>") == std::string::npos)
	{
		if (line.empty())
			getline(config, line);
		if (config.eof())
			throw std::logic_error("Unexpected end of ServerBlock");
		getline(config, line);
		std::cout << "server block: " << line << std::endl;
		if (line.find("<location>") != std::string::npos)
			checkLocationBlock(config, line);
		else if (line.find("<socket>") != std::string::npos)
			throw std::logic_error("ServerBlock cannot contain another SocketBlock start <server>");
		else if (line.find("</location>") != std::string::npos)
			throw std::logic_error("Incorrect LocationBlock format");
		else if (line.find("<server>") != std::string::npos)
			throw std::logic_error("ServerBlock cannot contain another ServerBlock start <server>");
	}
}

static void	checkSocketBlock(std::fstream &config, std::string &line)
{
	if (line != "<socket>")
		throw std::logic_error("SocketBlock has to start with <socket>");
	while (line != "</socket>")
	{
		if (line.empty())
			getline(config, line);
		if (config.eof())
			throw std::logic_error("Unexpected end of SocketBlock");
		getline(config, line);
		std::cout << "socket block: " << line << std::endl;
		if (line.find("<server>") != std::string::npos)
			checkServerBlock(config, line);
		else if (line.find("</server>") != std::string::npos)
			throw std::logic_error("Incorrect ServerBlock format");
		else if (line.find("<location>") != std::string::npos)
			throw std::logic_error("Incorrect LocationBlock format");
		else if (line.find("</location>") != std::string::npos)
			throw std::logic_error("Incorrect LocationBlock format");
		if (line.find("<socket>") != std::string::npos)
			throw std::logic_error("SocketBlock cannot contain another SocketBlock start <socket>");
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
			checkSocketBlock(config, line);
			server_counter++;
		}
	}
	std::cout << server_counter << " Sockets\n";
	config.close();
	return server_counter;
}
