#include <iostream>
#include <fcntl.h>
#include <string>
#include <fstream>
#include <stdexcept>

int	checkServerBlock(std::fstream	&istream, std::string	&line)
{
	while (line != "</server>")
	{
		getline(istream, line);
		if 
	}
	return 1;
}

int	checkConfig(std::string PATH)
{
	std::fstream	istream;
	std::string		line;
	int				server_counter;

	istream.open(PATH, std::fstream::in);

	while (!getline(istream, line).eof())
	{
		if (!line.empty() && checkServerBlock(istream, line))
			server_counter++;
	}
	return server_counter;
}

int main()
{
	checkConfig("./sample.config");
	return 1;
}
