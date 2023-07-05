#include "../includes/WebServer.hpp"

int main (int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Error: invalid number of arguments, please enter one argument\n";
		exit(EXIT_FAILURE);
	}
	else
	{
		FILE* file = fopen(argv[1], "r");
		if (!file)
		{
		    std::cerr << "Error opening file: " << argv[1] << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	WebServer server(argv[1]);
	server.setupListeningSockets();
	server.loopPollEvents();
}
