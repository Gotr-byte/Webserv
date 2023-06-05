#include "../includes/HTTP_server.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ConfigCheck.hpp"
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <fstream>

#define BUF_SIZE 1024

/**
 * Constructor of HTTP server, creates an array of client
 * 
 * @param filename The name of the file to read.
 * @return The contents of the file as a string.
 */
HTTP_server::HTTP_server()
{
	
}

HTTP_server::~HTTP_server(){}

/**
 * Reads the contents of a file and returns it as a string.
 * 
 * @param filename The name of the file to read.
 * @return The contents of the file as a string.
 */
std::string HTTP_server::read_file(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	std::string content;
	char c;
	if (!file)
	{
		std::cerr << "Error opening file " << filename << std::endl;
		return "";
	}
	while (file.get(c))
	{
		content += c;
	}
	file.close();
	return content;
}

/**
 * Tokenizes a line of text and stores the key-value pair in a map.
 * 
 * @param request The map to store the key-value pair.
 * @param line_to_tokenize The line of text to tokenize.
 */
void HTTP_server::tokenizing(std::map<std::string, std::string> &request, std::string line_to_tokenize)
{
	std::stringstream tokenize_stream(line_to_tokenize);
	std::string value;
	std::string key;
	key = line_to_tokenize.substr(0, line_to_tokenize.find(":") + 1);
	value = line_to_tokenize.substr(line_to_tokenize.find(":") + 1);
	removeWhitespaces(key);
	removeWhitespaces(value);
	request[key] = value;
}

void HTTP_server::removeWhitespaces(std::string &string)
{
	string.erase(0, string.find_first_not_of(" \t"));
	string.erase(string.find_last_not_of(" \t") + 1);
}

/**
 * Creates the pollfd structures for polling file descriptors.
 * 
 * The function initializes the `fds` array with the listening socket file descriptors and sets the events to poll for.
 */
void HTTP_server::create_pollfd_struct(void)
{
	memset(fds, 0, sizeof(fds));
	for (int i = 0; i < listening_port_no; i++)
	{
		fds[i].fd = listening_socket_fd[i];
		fds[i].events = POLLIN | POLLOUT;
	}
}

void HTTP_server::server_port_listening(int i)
{
	if (fds[i].revents & POLLIN)
	{
		Client client_fd;
		client_len = sizeof(client_addr);
		client_fd.fd = accept(listening_socket_fd[i], (struct sockaddr *)&client_addr, &client_len);
		if (client_fd.fd < 0)
		{
			perror("Error accepting client connection on etc");
			exit(EXIT_FAILURE);
		}
		client_fd.socket = i;
		clients.push_back(client_fd);
		fds[clients.size() + listening_port_no - 1].fd = client_fd.fd;
		fds[clients.size() + listening_port_no - 1].events = POLLIN | POLLOUT;
		fds[clients.size() + listening_port_no - 1].revents = 0;
		std::cout << "New client connected on etc fd on socket: " << i << "\n";
	}
}

void HTTP_server::server_conducts_poll()
{
	nfds = clients.size() + listening_port_no;
	res = poll(fds, nfds, 10);
	if (res < 0)
	{
		perror("Error polling sockets");
		exit(EXIT_FAILURE);
	}
}

void HTTP_server::server_mapping_request(int i)
{
	std::string key;
	char buf[BUF_SIZE];
	memset(buf, 0, BUF_SIZE);
	if (!clients[i].request.empty())
		clients[i].request.clear();
	int n = recv(clients[i].fd, buf, BUF_SIZE, 0);
	if (n < 0)
	{
		perror("Error receiving data from client");
		exit(EXIT_FAILURE);
	}
	std::string HTTP_request(buf);
	line = std::strtok(&HTTP_request[0], "\n");
	while (line != NULL)
	{
		std::string strLine(line);
		std::cout << strLine << "\n";
		lines.push_back(strLine);
		line = strtok(NULL, "\n");
	}
	if (!lines.empty())
	{
		clients[i].request["method:"] = std::strtok(&lines.front()[0], " ");
		clients[i].request["location:"] = std::strtok(NULL, " ");
		clients[i].request["HTTP_version:"] = std::strtok(NULL, " ");
		// clients[i].request["Host:"] = std::strtok(NULL, " ");
	}
	int new_line_count = 0;
	while (!lines.empty())
	{
		if (!lines.empty())
		{
			if (lines.front() == "\n" && new_line_count == 1)
			{
				lines.pop_front();
				break;
			}
			if (lines.front() == "\r")
			{
				new_line_count++;
			}
			else
			{
				tokenizing(clients[i].request, lines.front());
			}
		}
		lines.pop_front();
	}
}

void HTTP_server::perform_get_request(int i)
{
	// Check if the initial response headers have been sent for this client
	if (!clients[i].initialResponseSent)
	{
		// Send the initial headers
		std::cout << clients[i].response.ResponseHeader.c_str() << std::endl;
		if (send(clients[i].fd, clients[i].response.ResponseHeader.c_str(), clients[i].response.ResponseHeader.length(), 0) < 0)
		{
			perror("Error sending initial clients[i].response headers");
			exit(EXIT_FAILURE);
		}
		// Check if Autoindex Page is Requested and send it
		if (clients[i].response.path == "AUTOINDEX")
		{
			if (send(clients[i].fd, clients[i].response.autoindexbody.c_str(), clients[i].response.autoindexbody.size(), 0)  == -1)
				std::cout << "Autoindex send error" << std::endl;
			clients.erase(clients.begin() + i);
			return ;
		}
		int file_fd = open(clients[i].response.path.c_str(), O_RDONLY);
		if (file_fd < 0)
		{
			perror("Error opening file");
			exit(EXIT_FAILURE);
		}
		clients[i].file_fd = dup(file_fd);
		close(file_fd);
		// Mark the initial response as sent for this client
		clients[i].initialResponseSent = true;

		// Store the file descriptor and content length for subsequent chunked transfers
		// clients[i].file_fd = dup(file_fd);
		clients[i].content_length = clients[i].response.contentlength;
	}

	// Send the next chunk of data
	const int chunkSize = 1024; // Chunk size for each chunk
	char buffer[chunkSize];
	ssize_t bytesRead;
	bytesRead = read(clients[i].file_fd, buffer, chunkSize);
	if (bytesRead > 0)
	{
		std::cout << "sent " << bytesRead << "bytes to: " << clients[i].fd << "\n";
		// Prepare the chunk size and chunk data
		std::stringstream chunkSizeHex;
		chunkSizeHex << std::hex << bytesRead << "\r\n";
		std::string chunkSizeHexStr = chunkSizeHex.str();
		std::string chunkData(buffer, bytesRead);
		std::string chunk = chunkSizeHexStr + chunkData + "\r\n";

		// Send the chunk
		ssize_t bytesSent = send(clients[i].fd, chunk.c_str(), chunk.length(), 0);
		if (bytesSent < 0)
		{
			perror("Error sending chunk");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		// No more data to read, send the last chunk to indicate the end of the response
		std::string lastChunk = "0\r\n\r\n";
		if (send(clients[i].fd, lastChunk.c_str(), lastChunk.length(), 0) < 0)
		{
			perror("Error sending last chunk");
			exit(EXIT_FAILURE);
		}

		// Close the file descriptor
		close(clients[i].file_fd);

		// Remove the client from the list
		clients.erase(clients.begin() + i);
	}
}

std::string HTTP_server::toHex(int value)
{
	std::stringstream stream;
	stream << std::hex << value;
	return stream.str();
}

void HTTP_server::server_loop()
{
	int m = 0;
	while (true)
	{
		server_conducts_poll();
		for (int i = 0; i < listening_port_no; i++)
		{
			server_port_listening(i);
		}
		for (unsigned long i = 0; i < clients.size(); i++)
		{
			if (fds[i + listening_port_no].revents & POLLIN)
			{
				server_mapping_request(i);
				clients[i].CreateResponse(clients[i].request, ConfigVec[clients[i].socket]);
				if (i == 99999)
				{
					std::cout << "post is reached"
							  << "\n";

					// print_map();
					if (m < 0)
					{
						exit(EXIT_FAILURE);
					}
					while (!lines.empty())
					{
						std::cout << lines.front() << "\n";
						lines.pop_front();
					}
					const char *httpFileSentResponse =
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html\r\n"
						"Content-Length: 135\r\n"
						"\r\n"
						"<!DOCTYPE html>\r\n"
						"<html>\r\n"
						"<head>\r\n"
						"    <title>File Upload</title>\r\n"
						"</head>\r\n"
						"<body>\r\n"
						"    <h1>File Upload Successful</h1>\r\n"
						"    <p>Your file has been successfully uploaded to the server.</p>\r\n"
						"</body>\r\n"
						"</html>\r\n";
					int s = send(clients[i].fd, httpFileSentResponse, strlen(httpFileSentResponse), 0);
					if (s < 0)
					{
						perror("Error sending data to client");
						exit(EXIT_FAILURE);
					}
					close(clients[i].fd);
					clients.erase(clients.begin() + i);
				}
			}
			if (fds[i + listening_port_no].revents & POLLOUT)
			{
				if (clients[i].response.method == "GET")
					perform_get_request(i);
			}
		}
	}
	// Close all connected client sockets
	for (unsigned long i = 0; i < clients.size(); i++)
	{
		close(clients[i].fd);
	}
	// Close the server socket
	std::vector<int>::iterator it;
	for (it = listening_socket_fd.begin(); it != listening_socket_fd.end(); it++)
	{
		close(*it);
	}
}

int HTTP_server::handle_request(std::string path)
{

	ConfigCheck check;

	listening_port_no = check.checkConfig(path);

	for (int i = 0; i < listening_port_no; i++)
	{
		ServerConfig tmp(path, i);
		Socket socket(atoi(tmp.port.c_str()), tmp.host);
		listening_socket_fd.push_back(socket.server_fd);
		ConfigVec.push_back(tmp);
		std::cout << tmp.getConfProps("listen:") << std::endl;
		std::cout << " Socket " << (i + 1) << " (FD " << listening_socket_fd[i]
				  << ") is listening on: " << tmp.getConfProps("listen:") << std::endl;
	}
	create_pollfd_struct();
	server_loop();
	return 0;
}