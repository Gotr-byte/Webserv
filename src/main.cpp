#include "../includes/WebServer.hpp"


int main (int argc, char **argv, char **env)
{
    if (argc != 2)
    {
        std::cerr << "Error: invalid number of arguments, please enter one argument\n";
        return 1;
    }

    WebServer server(argv[1], env);

    server.setupListeningSockets();
    server.loopPollEvents();
}
