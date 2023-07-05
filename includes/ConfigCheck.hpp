#pragma once
#include <iostream>
#include <fcntl.h>
#include <string>
#include <fstream>
#include <stdexcept>

class ConfigCheck
{
	public:
		ConfigCheck();
		~ConfigCheck();

		int		checkConfig(std::string path);

	private:
		void	checkSocketBlock(std::fstream	&config, std::string	&line);
		void	checkServerBlock(std::fstream	&config, std::string	&line);
		void	checkLocationBlock(std::fstream	&config, std::string	&line);

		int server_counter;
};