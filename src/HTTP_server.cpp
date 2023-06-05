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
    fds = new struct pollfd[MAX_CLIENTS];
    clients = new Client[MAX_CLIENTS];
    currently_served_quantity = 0;
}

HTTP_server::~HTTP_server() {}

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
    if (!file){
        std::cerr << "Error opening file " << filename << std::endl;
        return "";
    }
    while (file.get(c)){
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
    std::getline(tokenize_stream, key, ':');
    std::getline(tokenize_stream, value, ':');
    request[key] = value;
}

/**
 * Creates a listening socket for the server on the specified port.
 *
 * @param port The port on which the server should listen for incoming connections.
 */
void HTTP_server::create_listening_sock(int port)
{
    Socket socket(port);
    listening_socket_fd.push_back(socket.server_fd);
}

/**
 * Creates the pollfd structures for polling file descriptors.
 *
 * The function initializes the `fds` array with the listening socket file descriptors and sets the events to poll for.
 */
void HTTP_server::create_pollfd_struct(void)
{
    memset(fds, 0, MAX_CLIENTS * sizeof(fds));
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
        Client client_get_fd;
        int pending_connection;
        client_len = sizeof(client_addr);
        pending_connection = accept(listening_socket_fd[i], (struct sockaddr *)&client_addr, &client_len);
        if (pending_connection < 0){
            perror("Error accepting client connection on etc");
            exit(EXIT_FAILURE);
        }
        pending_connections.push_back(pending_connection);
        client_get_fd.fd = pending_connections.front();
        clients[++currently_served_quantity + listening_port_no - 1] = client_get_fd;
        fds[currently_served_quantity + listening_port_no - 1].fd = pending_connections.front();
        fds[currently_served_quantity + listening_port_no - 1].events = POLLIN | POLLOUT;
        fds[currently_served_quantity + listening_port_no - 1].revents = 0;
        pending_connections.pop_front();
        std::cout << "New client connected on etc fd\n";
    }
}

/*
*We can use the timeout with try and reset the poll if the timeout is down because of stability
*
*/
void HTTP_server::server_conducts_poll()
{
    nfds = currently_served_quantity + listening_port_no;
    timeout = (3 * 60 * 1000);
    res = poll(fds, nfds, timeout);
    if (res < 0){
        perror("Error polling sockets");
        exit(EXIT_FAILURE);
    }
}

void HTTP_server::server_mapping_request(int i)
{
    std::string key;
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    int n = recv(clients[i].fd, buf, BUF_SIZE, 0);
    if (n < 0){
        perror("Error receiving data from client");
        exit(EXIT_FAILURE);
    }
    std::string HTTP_request(buf);
    line = std::strtok(&HTTP_request[0], "\n");
    while (line != NULL){
        std::string strLine(line);
        lines.push_back(strLine);
        line = strtok(NULL, "\n");
    }
    if (!lines.empty()){
        clients[i].request["method:"] = std::strtok(&lines.front()[0], " ");
        clients[i].request["location:"] = std::strtok(NULL, " ");
        clients[i].request["HTTP_version:"] = std::strtok(NULL, " ");
    }
    int new_line_count = 0;
    while (!lines.empty())
    {
        if (!lines.empty())
        {
            if (lines.front() == "\n" && new_line_count == 1){
                lines.pop_front();
                break;
            }
            if (lines.front() == "\r"){
                new_line_count++;
            }
            else{
                tokenizing(clients[i].request, lines.front());
            }
        }
        lines.pop_front();
    }
}

void HTTP_server::perform_get_request(int i)
{
    if (clients[i].request["location:"].substr(0, 6) == "/file/")
    {
        // Check if the initial response headers have been sent for this client
        if (!clients[i].initialResponseSent){
            filename = ".." + clients[i].request["location:"];
            std::cout << filename << "\n";
            // test
            int file_fd_err = open(filename.c_str(), O_RDONLY);
            if (file_fd_err < 0)
            {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            int file_fd = open(filename.c_str(), O_RDONLY);
            if (file_fd < 0)
            {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            clients[i].file_fd = dup(file_fd);
            close(file_fd);
            off_t content_length = lseek(clients[i].file_fd, 0, SEEK_END);
            lseek(clients[i].file_fd, 0, SEEK_SET);
            std::stringstream response_headers;
            response_headers << "HTTP/1.1 200 OK\r\n"
                             << "Transfer-Encoding: chunked\r\n"
                             << "Content-Type: application/octet-stream\r\n"
                             << "\r\n";

            std::string http_response = response_headers.str();
            if (send(clients[i].fd, http_response.c_str(), http_response.length(), 0) < 0)
            {
                perror("Error sending initial response headers");
                exit(EXIT_FAILURE);
            }
            clients[i].initialResponseSent = true;
            clients[i].content_length = content_length;
        }
        const int chunkSize = 1024;
        char buffer[chunkSize];
        ssize_t bytesRead;
        bytesRead = read(clients[i].file_fd, buffer, chunkSize);
        if (bytesRead > 0)
        {
            std::cout << "sent " << bytesRead << "bytes to: " << clients[i].fd << "\n";
            std::stringstream chunkSizeHex;
            chunkSizeHex << std::hex << bytesRead << "\r\n";
            std::string chunkSizeHexStr = chunkSizeHex.str();
            std::string chunkData(buffer, bytesRead);
            std::string chunk = chunkSizeHexStr + chunkData + "\r\n";
            ssize_t bytesSent = send(clients[i].fd, chunk.c_str(), chunk.length(), 0);
            if (bytesSent < 0){
                perror("Error sending chunk");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            std::string lastChunk = "0\r\n\r\n";
            if (send(clients[i].fd, lastChunk.c_str(), lastChunk.length(), 0) < 0)
            {
                perror("Error sending last chunk");
                exit(EXIT_FAILURE);
            }
            close(clients[i].file_fd);
            close(clients[i].fd);
            clients[i].initialResponseSent = false;
            clients[i].request.clear();
            currently_served_quantity--;
        }
    }
    else if (clients[i].request["location:"].substr(0, 6) == "/HTML/")
    {
        if (!clients[i].initialResponseSent)
        {
            filename = ".." + clients[i].request["location:"];
            std::cout << filename << "\n";
            // test
            int file_fd_err = open(filename.c_str(), O_RDONLY);
            if (file_fd_err < 0)
            {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            // err
            int file_fd = open(filename.c_str(), O_RDONLY);
            if (file_fd < 0)
            {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            clients[i].file_fd = dup(file_fd);
            close(file_fd);
            off_t content_length = lseek(clients[i].file_fd, 0, SEEK_END);
            lseek(clients[i].file_fd, 0, SEEK_SET);
            std::stringstream response_headers;
            response_headers << "HTTP/1.1 200 OK\r\n"
                             << "Transfer-Encoding: chunked\r\n"
                             << "Content-Type: text/html\r\n"
                             << "\r\n";

            std::string http_response = response_headers.str();
            if (send(clients[i].fd, http_response.c_str(), http_response.length(), 0) < 0)
            {
                perror("Error sending initial response headers");
                exit(EXIT_FAILURE);
            }
            clients[i].initialResponseSent = true;
            clients[i].content_length = content_length;
        }
        const int chunkSize = 1024;
        char buffer[chunkSize];
        ssize_t bytesRead;
        bytesRead = read(clients[i].file_fd, buffer, chunkSize);
        if (bytesRead > 0)
        {
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
            std::string lastChunk = "0\r\n\r\n";
            if (send(clients[i].fd, lastChunk.c_str(), lastChunk.length(), 0) < 0)
            {
                perror("Error sending last chunk");
                exit(EXIT_FAILURE);
            }

            close(clients[i].file_fd);
            close(clients[i].fd);
            clients[i].initialResponseSent = false;
            clients[i].request.clear();
            currently_served_quantity--;
        }
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
    while (true)
    {
        server_conducts_poll();
        for (int i = 0; i < listening_port_no; i++)
            server_port_listening(i);
        for (unsigned long i = listening_port_no; i < MAX_CLIENTS; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                server_mapping_request(i);
                // std::cout << "show me the location: " << clients[i].request["location:"];
                if (clients[i].request["method:"].substr(0, 4) == "POST")
                {
                    std::cout << "post is reached\n";
                }
            }
            if (fds[i].revents & POLLOUT)
            {
                if (clients[i].request["method:"].substr(0, 3) == "GET")
                    perform_get_request(i);
            }
        }
    }
    // Close all connected client sockets
    for (unsigned long i = 0; i < currently_served_quantity + listening_port_no; ++i){
        close(clients[i].fd);
    }
    // Close the server socket
    for(unsigned int i = 0; i < MAX_CLIENTS; ++i){
        close(fds[i].fd);
    }
}

int HTTP_server::handle_request(std::string path)
{

    ConfigCheck check;

    listening_port_no = check.checkConfig(path);

    for (int i = 0; i < listening_port_no; i++)
    {
        ServerConfig tmp(path, i);
        std::string port = tmp.getConfProps("port:");
        configVec.insert(std::make_pair(port, tmp));
        create_listening_sock(atoi(port.c_str()));
        std::cout << " Socket " << (i + 1) << " (FD " << listening_socket_fd[i]
                  << ") is listening on port: " << port << std::endl;
    }
    create_pollfd_struct();
    server_loop();
    return 0;
}