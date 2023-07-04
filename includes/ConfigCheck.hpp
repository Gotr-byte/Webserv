#pragma once
#include <iostream>
#include <fcntl.h>
#include <string>
#include <fstream>
#include <stdexcept>

class ConfigCheck
{
	public:
	int		checkConfig(std::string path);
};