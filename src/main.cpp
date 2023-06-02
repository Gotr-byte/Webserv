#include "../includes/HTTP_server.hpp"


int main (int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Error: invalid number of arguments, please enter one argument\n";
        return 1;
    }

    HTTP_server serverLoop;

    serverLoop.handle_request(argv[1]);
}
