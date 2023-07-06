#pragma once
#include <iostream>
#include <fcntl.h>
#include <string>
#include <fstream>
#include <stdexcept>
#include <stdlib.h>
#include <cstdlib>

class ConfigCheck
{
	public:
	int		checkConfig(std::string path);
};