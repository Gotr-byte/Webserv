#include "../includes/Socket.hpp"

Socket::Socket(int port, std::string ip)
{
	int opt = 1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE;     // For server sockets

	std::stringstream   port_s;
	port_s << port;

	int result = getaddrinfo(ip.c_str(), port_s.str().c_str(), &hints, &server_info);
	if (result != 0)
	{
		std::cerr << "Getaddrinfo Error";
		exit(EXIT_FAILURE);
	}

	server_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	if (server_fd < 0)
	{
		std::cerr << "socket: Error creating server socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "Error setting server socket to non-blocking" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error setting server socket options" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (bind(server_fd, server_info->ai_addr, server_info->ai_addrlen) < 0)
	{
		std::cerr << "Error binding server socket port" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 42) < 0)
	{
		std::cerr << "Error listening on server socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(server_info);
}

Socket::~Socket()
{}
 