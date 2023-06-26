#include "../includes/HTTP_server.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ConfigCheck.hpp"
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <fstream>

#define BUF_SIZE 1024
#define TIMEOUT 20

//TODO same server port different name, different methods allowed, second server runs
//TODO two servers sharing the same port, just ignore one of the servers
//TODO possibly there is a necessity to reduce the number of recv
//TODO remove perrors where it is not according to documentation


size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] != '\0')
		i++;
	return (i);
}


char	*ft_strchr(const char *s, int c)
{
	size_t	i;
	char	*h;
	size_t	t;

	t = ft_strlen(s);
	h = (char *)s;
	i = 0;
	while (i <= t)
	{
		if (*(h + i) == (char)c)
			return (h + i);
		i++;
	}
	return (0);
}


void HTTP_server::print_request(std::map<std::string, std::string> my_map)
{
    std::map<std::string, std::string>::iterator it;
    switch (color_index) {
        case 0:
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                std::cout << RED << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
            }
            color_index++;
            break;
        case 1:
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                std::cout << YELLOW << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
            }
            color_index++;
            break;
        case 2:
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                std::cout << CYAN << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
            }
            color_index++;
            break;
        case 3:
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                std::cout << CYAN << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
            }
            color_index = 0;
            break;
        default:
            std::cout << "Invalid choice." << std::endl;
            break;
    }
}

/**
 * Constructor of HTTP server, creates an array of client
 *
 * @param filename The name of the file to read.
 * @return The contents of the file as a string.
 */
HTTP_server::HTTP_server(std::string path, char **env): _env(env), _path(path)
{
    ConfigCheck check;
    listening_port_no = check.checkConfig(_path);
    color_index = 0;
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
 * Creates the pollfd structures for polling file descriptors.
 *
 * The function initializes the `fds` array with the listening socket file descriptors and sets the events to poll for.
 */
// void HTTP_server::create_pollfd_struct(void)
// {
//     memset(fds, 0, MAX_CLIENTS + 1 * sizeof(fds));
//     for (int i = 0; i < listening_port_no + MAX_CLIENTS + 1; i++)
//     {
//         fds[i].fd = FdsClients[i].first;
//         fds[i].events = POLLIN | POLLOUT;
//     }
// }

void HTTP_server::server_port_listening(int i)
{
    if (pollfds[i].revents & POLLIN)
    {
        client_len = sizeof(client_addr);
        //TODO add address to client struct
        int client_fd;
        client_fd = accept(pollfds[i].fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            perror("Error accepting client connection on etc");
            exit(EXIT_FAILURE);
        }
        FdClients.insert(std::make_pair(client_fd, Client(i, ConfigVec[i])));

        struct pollfd pollstruct;
        pollstruct.fd = client_fd;
        pollstruct.events = POLLIN | POLLOUT;
        pollstruct.revents = 0;
        this->pollfds.push_back(pollstruct);
        std::cout << "New client connected\n";
    }
}

/*
 *We can use the timeout with try and reset the poll if the timeout is down because of stability
 *
 */
void HTTP_server::server_conducts_poll()
{
    // timeout = (3 * 60 * 1000);
    timeout = -1;
    res = poll(pollfds.data(), pollfds.size(), timeout);
    if (res < 0)
    {
        perror("Error polling sockets");
        exit(EXIT_FAILURE);
    }
}

//also update location request
void HTTP_server::generate_cgi_querry(std::map<std::string, std::string>&new_request){
    char * temporary = std::strtok(&new_request["location:"][0], "?");
    std::cout << "the temp file: "<< temporary << "\n";
    new_request["query_string:"] = std::strtok(NULL, "");
    new_request["location:"] = temporary;
}

// size_t HTTP_server::findHeaderLength(int fd)
// {
//     char buf[BUF_SIZE];
//     memset(buf, 0, BUF_SIZE);
//     // try{
//     int n = recv(fd, buf, BUF_SIZE, MSG_PEEK);
//     if (n < 0){
//         perror("Header lenght error:");
//         // throw(HeaderLengthException());
//     }
//     // }
//     // catch (const std::exception &e){
//     //             std::cerr << e.what();
//     // }
//     // catch
//     char *header = std::strstr(buf, "\r\n\r\n");
//     size_t headerlength = header - buf + 4;
//     return headerlength;
// }

// std::map<std::string, std::string> HTTP_server::mapping_request_header(std::string Request)
// {
//     std::string key;
//     std::map<std::string, std::string> new_request;
//     size_t headerlength = findHeaderLength(FdsClients[i].first);
//     int new_line_count = 0;
//     char buf[BUF_SIZE];
//     memset(buf, 0, headerlength);

//     // Get lines of Header
//     int n;
//     n = recv(FdsClients[i].first, buf, headerlength, MSG_DONTWAIT);
//     if (n < 0)
//     {
//         perror("Error receiving data from in server_mapping_request");
//         exit(EXIT_FAILURE);
//     }
//     std::string HTTP_request(buf);
//     line = std::strtok(&HTTP_request[0], "\n");
//     while (line != NULL)
//     {
//         std::string strLine(line);
//         lines.push_back(strLine);
//         line = strtok(NULL, "\n");
//     }
//     if (!lines.empty())
//     {
//         new_request["method:"] = std::strtok(&lines.front()[0], " ");
//         new_request["location:"] = std::strtok(NULL, " ");
//         std::size_t found = new_request["location:"].find('?');
//         if (found != std::string::npos && new_request["method:"] == "GET"){
//             new_request["HTTP_version:"] = std::strtok(NULL, " ");
//             std::string temporary = std::strtok(&new_request["location:"][0], "?");
//             new_request["query_string:"] = std::strtok(NULL, " ");
//             std::cout << "test1\n";
//             new_request["location:"] = temporary;
//             std::cout << "test1\n";
//         }
//         else
//             new_request["HTTP_version:"] = std::strtok(NULL, " ");
//     }
//     while (!lines.empty())
//     {
//         if (!lines.empty() &&
//             lines.front() == "\n" &&
//             new_line_count == 1){
//             lines.pop_front();
//             break;
//         }
//         if (!lines.empty() &&
//             lines.front() == "\r"){
//             new_line_count++;
//         }
//         else{
//             tokenizing(new_request, lines.front());
//         }
//         lines.pop_front();
//     }
//     size_t v;
//     if ((v = new_request["Content-Type:"].find("boundary=")) != std::string::npos)
//     {
//         std::string key = new_request["Content-Type:"].substr(v, 8);
//         std::string value = new_request["Content-Type:"].substr(new_request["Content-Type:"].find("=") + 1);
//         new_request[key] = value;
//         new_request["Content-Type:"] = new_request["Content-Type:"].substr(0, new_request["Content-Type:"].find(";"));
//     }
//     if (new_request["method:"] != "POST")
//     {
//         memset(buf, 0, BUF_SIZE);
//         recv(FdsClients[i].first, buf, BUF_SIZE, MSG_DONTWAIT);
//    }
//     print_request(new_request);
//     return new_request;
// }

// void HTTP_server::ProcessUpload(std::vector<Request>::iterator req)
// {
//     size_t headerlength = 0;

//     char buf[BUF_SIZE];
//     memset(buf, 0, headerlength);

//     // Get lines of Header
//     int n;
//     n = recv(req->client_fd, buf, headerlength, MSG_DONTWAIT);
//     if (n < 0)
//     {
//         perror("Error receiving data from in upload");
//         exit(EXIT_FAILURE);
//     }
    
//     std::string fileheader(buf);

//     std::string filename = fileheader.substr(fileheader.find("filename=") + 10);
//     filename = filename.substr(0, filename.find("\""));
//     req->path += filename;
//     std::ofstream createFile(req->path, std::ios::binary | std::ios::trunc);

//     if (!createFile.is_open())
//     {
//         perror("error creating upload file");
//         exit(1);
//     }

//     size_t bodyLength = std::atol(req->requestHeader["Content-Length:"].c_str()) - headerlength;
//     size_t boundaryLength = req->requestHeader["boundary"].size() + 7;
//     size_t readbytes = 0;
//     size_t remainingBytes = bodyLength - boundaryLength;

//     while (remainingBytes > boundaryLength)
//     {
//         size_t chunkBytes = std::min((size_t) BUF_SIZE, remainingBytes);

//         memset(buf, 0, chunkBytes);
//         readbytes = recv(req->client_fd, buf, chunkBytes, MSG_DONTWAIT);
//         if (readbytes < 0)
//         {
//             perror("Error receiving data from client in Upload");
//             exit(EXIT_FAILURE);
//         }

//         createFile.write(buf, readbytes);
//         remainingBytes -= readbytes;
//     }
//     createFile.close();

//     //Read rest of the request body and dicscard it
//     memset(buf, 0, BUF_SIZE);
//     recv(req->client_fd, buf, BUF_SIZE, MSG_DONTWAIT);
//     req->GenerateUploadResponse();
// }

void HTTP_server::send_response(int client_fd)
{
    if (!FdClients.at(client_fd).header_sent)
    {
        if (FdClients.at(client_fd).response.body.empty())
        {
            int file_fd = open(FdClients.at(client_fd).path_on_server.c_str(), O_RDONLY);
            if (file_fd < 0)
            {
                perror("Error opening file");
                throw(InvalidFileDownloadException());
            }
            FdClients.at(client_fd).file_fd = file_fd;
        }
        std::cout << FdClients.at(client_fd).response.header.c_str() << std::endl;
        if (send(client_fd, FdClients.at(client_fd).response.header.c_str(), FdClients.at(client_fd).response.header.size(), 0) < 0)
        {
            FdClients.at(client_fd).close_file_fd();
            perror("Error sending initial response headers");
            exit(EXIT_FAILURE);
        }
        FdClients.at(client_fd).header_sent = true;
        return;
    }
    else if (FdClients.at(client_fd).header_sent)
    {
        if (!FdClients.at(client_fd).response.body.empty())
        {
        std::cout << "TEST2" << std::endl;
            if (send(client_fd, FdClients.at(client_fd).response.body.c_str(), FdClients.at(client_fd).response.body.size(), 0) < 0)
                perror("Error sending custom body");
            FdClients.at(client_fd).response_sent = true;
            return;
        }
        else
        {
            std::cout << "TEST3" << std::endl;
            char buffer[PACKAGE_SIZE];
            ssize_t bytesRead;
            bytesRead = read(FdClients.at(client_fd).file_fd, buffer, PACKAGE_SIZE);
            if (bytesRead > 0)
            {
                std::stringstream chunkSizeHex;
                chunkSizeHex << std::hex << bytesRead << "\r\n";
                std::string chunkSizeHexStr = chunkSizeHex.str();
                std::string chunkData(buffer, bytesRead);
                std::string chunk = chunkSizeHexStr + chunkData + "\r\n";
                ssize_t bytesSent = send(client_fd, chunk.c_str(), chunk.length(), 0);
                if (bytesSent < 0)
                {
                    FdClients.at(client_fd).close_file_fd();
                    perror("Error sending chunk");
                    exit(EXIT_FAILURE);
                }
                std::cout << chunk.c_str() << std::endl;
            }
            else
            {
                std::cout << "TEST 4" << std::endl;
                FdClients.at(client_fd).close_file_fd();
                 std::cout << FdClients.at(client_fd).send_last_chunk << std::endl;
                FdClients.at(client_fd).send_last_chunk = true;
                 std::cout << FdClients.at(client_fd).send_last_chunk << std::endl;
            }
            return;
        }
    }
    else if (FdClients.at(client_fd).send_last_chunk)
    {
        std::string lastChunk = "0\r\n\r\n";
        if (send(client_fd, lastChunk.c_str(), lastChunk.length(), 0) < 0)
        {
            perror("Error sending last chunk");
            exit(EXIT_FAILURE);
        }
        FdClients.at(client_fd).response_sent = true;
        std::cout << "TEST5" << std::endl;
    }
}

std::string HTTP_server::toHex(int value)
{
    std::stringstream stream;
    stream << std::hex << value;
    return stream.str();
}

void HTTP_server::kill_client(std::vector<struct pollfd>::iterator it)
{
    close(it->fd);
    FdClients.erase(it->fd);
    pollfds.erase(it);
    std::cout << "client disconnected" << std::endl;
}

void HTTP_server::server_loop()
{
    while (true)
    {
        server_conducts_poll();
        for (int i = 0; i < listening_port_no; i++)
            server_port_listening(i);
        for (std::vector<struct pollfd>::iterator it = (pollfds.begin() + listening_port_no); it != pollfds.end(); it++)
        {
            if (it->revents & POLLIN)
            {
                char request_chunk[PACKAGE_SIZE + 1];
                memset(request_chunk, 0, PACKAGE_SIZE);
                ssize_t recieved_size = recv(it->fd, request_chunk, PACKAGE_SIZE, 0);
                if (recieved_size < 0)
                {
                    kill_client(it);
                    break;
                }
                else if (recieved_size == 0)
                {
                    kill_client(it);
                    break;
                }
                std::cout << request_chunk << std::endl;
                FdClients.at(it->fd).set_request(request_chunk, recieved_size);
                // mapping request header and deleting request header
                if (FdClients.at(it->fd).request_header.empty())
                {
                    FdClients.at(it->fd).mapping_request_header();
                    FdClients.at(it->fd).check_request();
                }
                if (FdClients.at(it->fd).is_error)
                    it->events = POLLOUT;
                else if (FdClients.at(it->fd).request_complete && !FdClients.at(it->fd).autoindex)
                {
                    if (FdClients.at(it->fd).method == "GET")
                        FdClients.at(it->fd).response.BuildResponseHeader();
                    std::cout << "perform delete, upload or cgi" << std::endl;
                    // if (FdClients.at(it->fd).isUpload)
                    //     ProcessUpload(FdClients.at(it->fd));
                }
            }
            //     else if (new_req.isCGI)
            //     {
            //         std::cout << "*******************\n";
            //         std::cout << "* Welcome to CGI! *\n";
            //         std::cout << "*******************\n";
            //         Cgi cgi("generic cgi", new_req.id);
            //         try{
            //             cgi.run(FdsClients[*it_idx].second.Requests.end() - 1);
            //         }
            //         catch (const std::exception &e){
            //             std::cerr << e.what();
            //         }
            //         (void)fds;
            //     }
            //     else if (new_req.isDelete)
            //         deleteContent(FdsClients[*it_idx].second.Requests.end() - 1);
            // }
            if (it->revents & POLLOUT)
            {
                try
                {
                    send_response(it->fd);
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what();
                }
                if (FdClients.at(it->fd).response_sent)
                {
                    std::cout << "Request: " << FdClients.at(it->fd).id << " sent to fd: " << it->fd << std::endl;
                    kill_client(it);
                    break;
                }
            }
        }
    }
    // for (std::set<int>::iterator it_idx = activeClientIdx.begin(); it_idx != activeClientIdx.end(); it_idx++)
    // {
    //     close(FdsClients[*it_idx].first);
    //     close(fds[*it_idx].fd);
    // }
}

// void HTTP_server::deleteContent(std::vector<Response>::iterator req)
// {
//     int i = std::remove(req->path.c_str());
//     if (i != 0)
//     {
//         req->GenerateClientErrorResponse("409", "Conflict");
//         return;
//     }
//     req->GenerateDeleteResponse();
// }

// void HTTP_server::InitFdsClients()
// {
//     ConfigCheck check;
//     listening_port_no = check.checkConfig(_path);

//     for (int i = 0; i < (MAX_CLIENTS + listening_port_no + 1); i++)
//     {
//         Client init;
//         std::pair<int, Client> tmp(-1, init);
//         FdsClients.push_back(tmp);
//     }
// }

int HTTP_server::running()
{
    //creating listening sockets
    for (int i = 0; i < listening_port_no; i++)
    {
        struct pollfd listening_poll;
        ServerConfig tmp(_path, i);
        Socket socket(std::atoi(tmp.port.c_str()), tmp.host);
        listening_poll.fd = socket.server_fd;
        listening_poll.events = POLLIN;
        listening_poll.revents = 0;
        pollfds.push_back(listening_poll);
        ConfigVec.push_back(tmp);
        std::cout << "Socket " << (i + 1) << " (FD " << socket.server_fd
                  << ") is listening on: " << tmp.getConfProps("listen:") << std::endl;
    }

    server_loop();

    // create_pollfd_struct();

    return 0;
}
