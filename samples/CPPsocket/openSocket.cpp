#include "webserv.hpp"

static int	init_socket_from_address_list(struct addrinfo *socket_addr_list)
{
	int	socket_fd;
	int optval = 1;
	const struct addrinfo *socket_addr_list_node;

	socket_addr_list_node = socket_addr_list;
	while (socket_addr_list_node != LIST_END)
	{
		socket_fd = socket(socket_addr_list_node->ai_family,
							socket_addr_list_node->ai_socktype, socket_addr_list_node->ai_protocol);
		if (socket_fd != FAILED_SOCKET) {
			setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
			if (bind(socket_fd, socket_addr_list_node->ai_addr, socket_addr_list_node->ai_addrlen) == SUCESS) {
				// limit_socket_buffer_intake(socket_fd);
				break;
			}
			close(socket_fd);
		}
		socket_addr_list_node = socket_addr_list_node->ai_next;
	}
	freeaddrinfo(socket_addr_list);
    if (socket_addr_list_node == NULL)
		// throw BindFailure();
	return (socket_fd);
}

/*
	getaddrinfo function wrapper
	https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
*/
static struct addrinfo *get_socket_address_info_list(const struct addrinfo *TCP_service_info, char const *port_number)
{
	struct addrinfo *result_addr_list;
	int				gai_error_code;

	gai_error_code = getaddrinfo(NULL, port_number, TCP_service_info, &result_addr_list);
	if (gai_error_code != SUCESS) {
		perror("getaddrinfo:\n");
		perror(gai_strerror(gai_error_code));
    	// throw BindFailure();
	}
	return (result_addr_list);
}

static void	set_addrinfo_for_http(struct addrinfo *TCP_service_info)
{
	memset(TCP_service_info, 0, sizeof(struct addrinfo));
	TCP_service_info->ai_family = AF_INET;
	TCP_service_info->ai_socktype = SOCK_STREAM;
	TCP_service_info->ai_flags = AI_PASSIVE;
	TCP_service_info->ai_protocol = 0;
}

static int bind_kernel_socket(const struct addrinfo *desired_TCP_service_info, char const *port_number)
{
	struct addrinfo *possible_socket_addr_list;

	possible_socket_addr_list = get_socket_address_info_list(desired_TCP_service_info, port_number);
	try
	{
		return (init_socket_from_address_list(possible_socket_addr_list));
	}
	catch (...)
	{
		std::cerr << "cannot bind" << std::endl;
		// throw BindFailure();
	}
}

// static int	learn_mtu_size(const int sockfd)
// {
// 	int mtu = 0;
// 	socklen_t mtu_len = sizeof(mtu);
// 	if (getsockopt(sockfd, SOL_SOCKET, SO_MAX_MSG_SIZE, &mtu, &mtu_len) < 0) {
// 		std::cerr << "Failed to get MTU" << std::endl;
// 		return 1;
// 	}
// 	std::cout << "MTU: " << mtu << std::endl;
// }

/*
	Starts socket in kernel, binds to it, and listen. -> returns fd of listening of port
*/
int	openPortSocket(char const *port_number)
{
	struct addrinfo desired_TCP_service_info;
	int				socket_fd;

	set_addrinfo_for_http(&desired_TCP_service_info);
	socket_fd = bind_kernel_socket(&desired_TCP_service_info, port_number);
	// if (listen(socket_fd, MAX_REQUEST_QUE) != SUCESS)
	// 	throw BindFailure();
	return (socket_fd);
}