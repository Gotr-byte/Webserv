#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sstream>

int	main(void)
{
	int			socket_fd, new_socket;
	int			port = 1010;
	sockaddr_in	address;
	size_t		add_len;

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		std::cout << "socket error" << std::endl;

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htons(INADDR_ANY);

	add_len = sizeof(address);

	if (bind(socket_fd, (struct sockaddr *)&address, add_len) == -1)
		std::cout << "bind error" << strerror(errno) << std::endl;
	
	if (listen(socket_fd, 10000))
		std::cout << "listen error" << strerror(errno) << std::endl;

	std::cout << "waiting for connections" << std::endl;
	int connections = 0;
	while (connections < 10)
	{
		char buffer[200] = "";
		if ((new_socket = accept(socket_fd, (struct sockaddr *)&address, (socklen_t *)&add_len)) < 0)
			std::cout << "accept error" << strerror(errno) << std::endl;
		read(new_socket, buffer, 200);
		std::cout << buffer;
		std::stringstream response;
		response << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
		int file_fd = open("./index.html", O_RDONLY);
		char content[1000] = "";
		response << read(file_fd, content, 1000) << "\n\n";
		close(file_fd);
		response << content;
		write(new_socket, response.str().c_str(), strlen(response.str().c_str()));
		sleep(2);
		if (close(new_socket) == -1)
			std::cout << "close error: " << strerror(errno) << std::endl;
		connections++;
	}
	if (close(socket_fd) == -1)
		std::cout << "close error: " << strerror(errno) << std::endl;
	return 0;
}