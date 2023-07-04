#include "../includes/Socket.hpp"
#include "../includes/ft_int_to_string.hpp"

// std::string int_to_string(int value) {
//     std::stringstream ss;
//     ss << value;
//     return ss.str();
// }

Socket::Socket()
{}

Socket::Socket(int port, std::string ip)
{
    int opt = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // For server sockets

    std::string port_str = ft_int_to_string(port);

    // Get address information for the server
    int result = getaddrinfo(ip.c_str(), port_str.c_str(), &hints, &server_info);
    if (result != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(result));
        exit(EXIT_FAILURE);
    }

    // Create server socket
    server_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (server_fd < 0)
    {
        std::cout << "socket: Error creating server socket\n";
        exit(EXIT_FAILURE);
    }

    // Set the socket to non-blocking
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        perror("Error setting server socket to non-blocking");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("Error setting server socket options");
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, server_info->ai_addr, server_info->ai_addrlen) < 0)
    {
        perror("Error binding server socket port");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections on the server socket
    if (listen(server_fd, 42) < 0)
    {
        perror("Error listening on server socket");
        exit(EXIT_FAILURE);
    }

    // Free address info
    freeaddrinfo(server_info);
}

Socket::~Socket()
{}
 