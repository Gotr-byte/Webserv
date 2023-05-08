#include "../includes/Configuration.hpp"

int Configuration::parseSetListen(std::string filename, std::string parameter){
    std::ifstream file(filename.c_str());

    if (!file)
    {
        std::cerr << "Unable to open file";
        return 1;
    }

    std::string value;
    while (file >> value)
    {
        if (value == parameter)
        {
            if (file >> value) // attempt to read the next value
            {
                this->listen = atoi(value.c_str());
                std::cout << "The value of listen '" << parameter << "' is '" << this->listen << "'\n";
                return 0;
            }
            else
            {
                std::cerr << "There is no vaule for '" << parameter << "', in the config file\n";
                return 1;
            }
        }
    }

    std::cout << "The value '" << parameter << "' was not found in the file\n";
    return(1);
}
