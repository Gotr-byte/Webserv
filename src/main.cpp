#include "../includes/Configuration.hpp"
#include "../includes/RequestHandler.hpp"
#include "../includes/ConfigCheck.hpp"


int main (int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Error: invalid number of arguments, please enter one argument\n";
        return 1;
    }
    ConfigCheck config_checker;
    Configuration conf;
    RequestHandler serverLoop;

    config_checker.checkConfig(argv[1]);
    if (conf.parseSetListen(argv[1], "port:"))
        return 2;
    std::cout << conf.listen << "\n";
    serverLoop.handleRequest(conf.listen);
}
