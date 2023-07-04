#include "../includes/WebServer.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ConfigCheck.hpp"
#include "../includes/ft_int_to_string.hpp"
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <fstream>

#define BUF_SIZE 1024
#define POLL_TIMEOUT 200

// std::string int_to_string(int value) {
//     std::stringstream ss;
//     ss << value;
//     return ss.str();
// }

bool deleteIfExists(const char* filename) {
    if (FILE* file = fopen(filename, "r")) {
        fclose(file);
        if (std::remove(filename) == 0) {
            return true; // File deleted successfully
        } else {
            return false; // Failed to delete the file
        }
    } else {
        return true; // File doesn't exist, consider it deleted
    }
}

/**
 * Constructor of HTTP server, creates an array of client
 *
 * @param filename The name of the file to read.
 * @return The contents of the file as a string.
 */
WebServer::WebServer(std::string path, char **env): _env(env), config_path(path)
{
    ConfigCheck check;
    listening_port_no = check.checkConfig(config_path);
}

WebServer::~WebServer()
{}

bool    WebServer::validateFilename(std::string filename)
{
    if (filename.empty() || 50 < filename.size())
        return false;
    for (std::size_t i = 0; i < filename.length(); i++) 
    {
        if (!isprint(static_cast<unsigned char>(filename[i])))
            return false;
    }
    if (filename.find("\\") != std::string::npos || filename.find("/") != std::string::npos)
        return false;
    return true;

}

void WebServer::performUpload(int client_fd)
{
    std::string upload_header;
    size_t upload_header_size;


    // std::cout << fds_clients.at(client_fd).request << std::endl;
    upload_header_size = fds_clients.at(client_fd).request.find("\r\n\r\n") + 4;
    upload_header = fds_clients.at(client_fd).request.substr(0, upload_header_size);

    std::string boundary_end = "\r\n--" + fds_clients.at(client_fd).request_header.at("boundary") + "--";


    std::string filename = upload_header.substr(upload_header.find("filename=") + 10);
    filename = filename.substr(0, filename.find("\""));
    if (!validateFilename(filename))
    {
        fds_clients.at(client_fd).setError("400");
        return;
    }
    std::string file_path = fds_clients.at(client_fd).path_on_server + filename;
    // std::ofstream createFile(file_path, std::ios::binary | std::ios::trunc);
    std::ofstream createFile(file_path.c_str(), std::ios::binary | std::ios::trunc);
    if (!createFile.is_open())
    {
        fds_clients.at(client_fd).setError("500");
        return;
    }

    size_t file_size = fds_clients.at(client_fd).request.rfind(boundary_end) - upload_header_size;

    createFile.write(fds_clients.at(client_fd).request.substr(upload_header_size, file_size).c_str(), file_size);
    if (createFile.fail() || createFile.bad())
    {
        fds_clients.at(client_fd).setError("500");
        return;
    }
    createFile.close();
    if (createFile.fail() || createFile.bad())
    {
        fds_clients.at(client_fd).setError("500");
        return;
    }

    fds_clients.at(client_fd).response.generateUploadResponse(file_path);
    fds_clients.at(client_fd).request_processed = true;
}

void WebServer::acceptClients(int server_fd)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    int client_fd;
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_size);
    if (client_fd < 0)
    {
        std::cerr << "webserver: Error accepting client connection\n";
        return;
    }

    fcntl(client_fd, F_SETFL, O_NONBLOCK);

    std::string client_ip = convertIPv4ToString(client_addr.sin_addr);

    fds_clients.insert(std::make_pair(client_fd, Client(configs.at(server_fd), client_ip)));
    struct pollfd pollstruct;
    pollstruct.fd = client_fd;
    pollstruct.events = POLLIN | POLLOUT;
    pollstruct.revents = 0;
    this->poll_fds.push_back(pollstruct);
}

std::string WebServer::convertIPv4ToString(const struct in_addr& address)
{
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&address.s_addr);
    std::string ipAddress;
    ipAddress += ft_int_to_string(static_cast<int>(bytes[0]));
    ipAddress += '.';
    ipAddress += ft_int_to_string(static_cast<int>(bytes[1]));
    ipAddress += '.';
    ipAddress += ft_int_to_string(static_cast<int>(bytes[2]));
    ipAddress += '.';
    ipAddress += ft_int_to_string(static_cast<int>(bytes[3]));
    return ipAddress;
}

void WebServer::conductPolling()
{
    if (poll(poll_fds.data(), poll_fds.size(), POLL_TIMEOUT) < 0)
    {
        std::cerr << "Error polling sockets\n";
        exit(EXIT_FAILURE);
    }
}

void    WebServer::performGet(int client_fd)
{
    fds_clients.at(client_fd).response.buildResponseHeader();
    fds_clients.at(client_fd).request_processed = true;
}

void WebServer::loopPollEvents()
{
    while (true)
    {
        conductPolling();
        for (std::vector<struct pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); it++)
        {
            if (it->revents & POLLIN)
            {
                if (it < poll_fds.begin() + listening_port_no)
                {
                    acceptClients(it->fd);
                    break;
                }
                else
                {
                    char request_chunk[PACKAGE_SIZE];
                    memset(request_chunk, 0, PACKAGE_SIZE);
                    ssize_t recieved_size = recv(it->fd, request_chunk, PACKAGE_SIZE, O_NONBLOCK);
                    if (recieved_size < 0)
                    {
                        fds_clients.at(it->fd).setError("500");
                        continue;
                    }
                    else if (recieved_size == 0)
                    {
                        std::cout << "Client closed the connection\n";
                        killClient(it--);
                        continue;
                    }
                    fds_clients.at(it->fd).setRequest(request_chunk, recieved_size);
                    // mapping request header and deleting request header
                    if (fds_clients.at(it->fd).request_header.empty() && fds_clients.at(it->fd).mapRequestHeader())
                        fds_clients.at(it->fd).checkRequest();
                    if (fds_clients.at(it->fd).cancel_recv)
                        it->events = POLLOUT;
                    if (!fds_clients.at(it->fd).request_processed && fds_clients.at(it->fd).request_complete)
                    {
                        if (fds_clients.at(it->fd).is_cgi)
                            performCgi(it->fd);
                        else if (fds_clients.at(it->fd).is_get)
                            performGet(it->fd);
                        else if (fds_clients.at(it->fd).is_delete)
                            performDelete(it->fd);
                        else if (fds_clients.at(it->fd).is_upload)
                            performUpload(it->fd);
                    }
                }
            }
            else if (it->revents & POLLOUT && fds_clients.at(it->fd).request_processed)
            {
                sendResponse(it->fd);
                if (fds_clients.at(it->fd).response_sent)
                {
                    deleteIfExists("./HTML/cgi-bin/city_of_brass");
                    killClient(it--);
                    continue;
                }
            }
            else if (it->revents & POLLHUP ||it->revents & POLLNVAL || it->revents & POLLERR)
            {
                std::cout << "POLLERR\n";
                killClient(it--);
                deleteIfExists("./HTML/cgi-bin/city_of_brass");
                continue;
            }
        }
    }
}

void    WebServer::performCgi(int client_fd)
{
    Cgi cgi(fds_clients.at(client_fd));
    try{
    cgi.run();
    }
    catch (const std::exception &e){
        std::cerr << e.what();
    }
    fds_clients.at(client_fd).request_processed = true;
    }

void WebServer::sendResponse(int client_fd)
{
    std::string chunk = "";

    if (!fds_clients.at(client_fd).header_sent)
    {
        if (fds_clients.at(client_fd).response.body.empty() && (!fds_clients.at(client_fd).is_delete && !fds_clients.at(client_fd).is_redirect))
        {
            int file_fd = open(fds_clients.at(client_fd).path_on_server.c_str(), O_RDONLY);
            if (file_fd < 0)
            {
                std::cout << "Error Opening File\n";
                fds_clients.at(client_fd).setError("500");
                return;
            }
            fds_clients.at(client_fd).file_fd = file_fd;
        }
        else
        {
            chunk = fds_clients.at(client_fd).response.header;
            if (!fds_clients.at(client_fd).is_delete && !fds_clients.at(client_fd).is_redirect)
                chunk += fds_clients.at(client_fd).response.body;
            if (send(client_fd, chunk.c_str(), chunk.size(), 0) < 0)
            {
                std::cout << "Error sending custom Body\n";
                fds_clients.at(client_fd).setError("500");
            }
            else
                fds_clients.at(client_fd).response_sent = true;
            return;
        }
    }
    char buffer[BUF_SIZE];
    ssize_t bytes_read;

    if (!fds_clients.at(client_fd).header_sent)
    {
        chunk += fds_clients.at(client_fd).response.header;
        fds_clients.at(client_fd).header_sent = true;
    }

    bytes_read = read(fds_clients.at(client_fd).file_fd, buffer, BUF_SIZE);
    if (bytes_read < 0)
    {
        fds_clients.at(client_fd).closeFileFd();
        fds_clients.at(client_fd).setError("500");
        return;
    }
    std::string read_data(buffer, bytes_read);

    if (fds_clients.at(client_fd).response.is_chunked)
    {
        std::stringstream hex_chunk_size;
        hex_chunk_size << std::hex << bytes_read << "\r\n";
        chunk += hex_chunk_size.str() + read_data + "\r\n";
    }
    else
        chunk += read_data;

    if (!fds_clients.at(client_fd).response.is_chunked || bytes_read == 0)
    {
        fds_clients.at(client_fd).closeFileFd();
        fds_clients.at(client_fd).last_chunk_sent = true;
    }
    ssize_t bytesSent = send(client_fd, chunk.c_str(), chunk.size(), 0);
    if (bytesSent < 0)
    {
        fds_clients.at(client_fd).closeFileFd();
        fds_clients.at(client_fd).setError("500");
    }
    else if (fds_clients.at(client_fd).last_chunk_sent)
        fds_clients.at(client_fd).response_sent = true;
}


std::string WebServer::toHex(int value)
{
    std::stringstream stream;
    stream << std::hex << value;
    return stream.str();
}

void WebServer::killClient(std::vector<struct pollfd>::iterator it)
{
    if ((close(it->fd)) < 0)
		std::cerr << "webserver loop: Error closing Socket Fd\n";
    fds_clients.erase(it->fd);
    poll_fds.erase(it);
}

void WebServer::performDelete(int client_fd)
{
    int i = std::remove(fds_clients.at(client_fd).path_on_server.c_str());
    if (i != 0)
    {
        fds_clients.at(client_fd).setError("409");
        return;
    }
    fds_clients.at(client_fd).response.generateDeleteResponse();
    fds_clients.at(client_fd).request_processed = true;
}

int WebServer::setupListeningSockets()
{
    for (int i = 0; i < listening_port_no; i++)
    {
        struct pollfd listening_poll;
        ServerConfig tmp(config_path, i);
        Socket socket(std::atoi(tmp.port.c_str()), tmp.host);
        listening_poll.fd = socket.server_fd;
        listening_poll.events = POLLIN;
        listening_poll.revents = 0;
        poll_fds.push_back(listening_poll);
        configs.insert(std::make_pair(listening_poll.fd, tmp));
        std::cout << "Socket " << (i + 1) << " (FD " << socket.server_fd
                  << ") is listening on: " << tmp.getConfProps("listen:") << std::endl;
    }
    return 0;
}
