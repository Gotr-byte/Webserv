#include "../includes/ConfigCheck.hpp"

static void checkFileExtension(const std::string& filename) {
    if (filename.length() < 7 || filename.substr(filename.length() - 7) != ".config") {
        std::cerr << "Error: Invalid file extension. Expected '.config'" << std::endl;
        exit(EXIT_FAILURE);
    }
}

static void checkFileEmpty(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (file.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "Error: File is empty." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

static void	checkLocationBlock(std::fstream &config, std::string &line)
{
	while (line.find("</location>") == std::string::npos)
	{
		if (line.empty())
			getline(config, line);
		if (config.eof())
		{
			std::cerr << "Config file: Unexpected end of LocationBlock" << std::endl;
			exit(EXIT_FAILURE);
		}
		getline(config, line);
		if (line.find("<server>") != std::string::npos)
		{
			std::cerr << "Config file: No ServerBlock allowed in LocationBlock" << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (line.find("<socket>") != std::string::npos)
		{
			std::cerr << "Config file: No SocketBlock allowed in LocationBlock" << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (line.find("<location>") != std::string::npos)
		{
			std::cerr << "Config file: LocationBlock cannot contain another LocationBlock start <location>" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
}

static void	checkServerBlock(std::fstream	&config, std::string	&line)
{
	while (line.find("</server>") == std::string::npos)
	{
		if (line.empty())
			getline(config, line);
		if (config.eof())
		{
			std::cerr << "Config file: Unexpected end of ServerBlock" << std::endl;
			exit(EXIT_FAILURE);
		}
		getline(config, line);
		if (line.find("<location>") != std::string::npos)
			checkLocationBlock(config, line);
		else if (line.find("<socket>") != std::string::npos)
		{
			std::cerr << "Config file: ServerBlock cannot contain another SocketBlock start <server>" << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (line.find("</location>") != std::string::npos)
		{
			std::cerr << "Config file: Incorrect LocationBlock format" << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (line.find("<server>") != std::string::npos)
		{
			std::cerr << "Config file: ServerBlock cannot contain another ServerBlock start <server>" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
}

static void	checkSocketBlock(std::fstream &config, std::string &line)
{
	if (line != "<socket>")
	{
		std::cerr << "Config file: SocketBlock has to start with <socket>" << std::endl;
		exit(EXIT_FAILURE);
	}
	while (line != "</socket>")
	{
		if (line.empty())
			getline(config, line);
		if (config.eof())
		{
			std::cerr << "Config file: Unexpected end of SocketBlock" << std::endl;
			exit(EXIT_FAILURE);
		}
		getline(config, line);
		if (line.find("<server>") != std::string::npos)
			checkServerBlock(config, line);
		else if (line.find("</server>") != std::string::npos)
		{
			std::cerr << "Config file: Incorrect ServerBlock format" << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (line.find("<location>") != std::string::npos)
		{
			std::cerr << "Config file: Incorrect LocationBlock format" << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (line.find("</location>") != std::string::npos)
		{
			std::cerr << "Config file: Incorrect LocationBlock format" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (line.find("<socket>") != std::string::npos)
		{
			std::cerr << "Config file: SocketBlock cannot contain another SocketBlock start <socket>" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
}

int	ConfigCheck::checkConfig(std::string path)
{
	std::fstream	config;
	std::string		line;
	int				server_counter = 0;

	checkFileExtension(path);
	checkFileEmpty(path);
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