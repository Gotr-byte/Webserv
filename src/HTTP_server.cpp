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
HTTP_server::HTTP_server(){
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
void HTTP_server::create_pollfd_struct(void){
    memset(fds, 0, MAX_CLIENTS * sizeof(fds));
    for (int i = 0; i < listening_port_no + MAX_CLIENTS + 1; i++){
        fds[i].fd = FdClientVec[i].first;
        fds[i].events = POLLIN | POLLOUT;
    }
}

void HTTP_server::server_port_listening(int i)
{
    if (fds[i].revents & POLLIN)
    {
        for (int j = listening_port_no; j < MAX_CLIENTS + listening_port_no + 1; j++)
        {
            if (fds[j].fd == -1)
            {
                std::cout << FdClientVec[i].first << std::endl;
                client_len = sizeof(client_addr);
                FdClientVec[j].first = accept(FdClientVec[i].first, (struct sockaddr *)&client_addr, &client_len);
                std::cout << "client FD: " << FdClientVec[j].first << "Index of client FD: " << j << std::endl;
                if (FdClientVec[j].first < 0)
		        {
                    perror("Error accepting client connection on etc");
                    exit(EXIT_FAILURE);
                }
                fds[j].fd = FdClientVec[j].first;
                FdClientVec[j].second.socket = i;
                activeClientIdx.insert(j);
                if (j == MAX_CLIENTS + listening_port_no)
                    FdClientVec[j].second.server_full = true;
                break ;
            }
        }
        std::cout << "New client connected\n";
    }
}

/*
*We can use the timeout with try and reset the poll if the timeout is down because of stability
*
*/
void HTTP_server::server_conducts_poll(){
    nfds = MAX_CLIENTS + listening_port_no + 1;
    // timeout = (3 * 60 * 1000);
    timeout = -1;
    res = poll(fds, nfds, timeout);
    if (res < 0){
        perror("Error polling sockets");
        exit(EXIT_FAILURE);
    }
}

void HTTP_server::server_mapping_request(int i){
    std::string key;
    int new_line_count = 0;
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    int n = recv(FdClientVec[i].first, buf, BUF_SIZE, 0);
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
        FdClientVec[i].second.request["method:"] = std::strtok(&lines.front()[0], " ");
        FdClientVec[i].second.request["location:"] = std::strtok(NULL, " ");
        FdClientVec[i].second.request["HTTP_version:"] = std::strtok(NULL, " ");
    }
    while (!lines.empty())
    {
        if (!lines.empty() && 
        lines.front() == "\n" && 
        new_line_count == 1){
            lines.pop_front();
            break;
        }
        if (!lines.empty() &&
        lines.front() == "\r"){
            new_line_count++;
        }
        else{
            tokenizing(FdClientVec[i].second.request, lines.front());
        }
        lines.pop_front();
    }
}

void HTTP_server::get_request(int i){
    if (!FdClientVec[i].second.initialResponseSent)
	{
		if (FdClientVec[i].second.response.path != "AUTOINDEX")
		{
	        int file_fd = open(FdClientVec[i].second.response.path.c_str(), O_RDONLY);
	        if (file_fd < 0)
			{
	            close(FdClientVec[i].second.file_fd);
	            close(FdClientVec[i].first);
	            FdClientVec[i].second.initialResponseSent = false;
	            FdClientVec[i].second.request.clear();
	            currently_served_quantity--;
	            perror("Error opening file");
	            throw(InvalidFileDownloadException());
	        }
        FdClientVec[i].second.file_fd = dup(file_fd);
        close(file_fd);
		}

        if (send(FdClientVec[i].first, FdClientVec[i].second.response.ResponseHeader.c_str(), FdClientVec[i].second.response.ResponseHeader.length(), 0) < 0)
		{
            perror("Error sending initial response headers");
            exit(EXIT_FAILURE);
        }

		if (FdClientVec[i].second.response.path == "AUTOINDEX")
		{
			if (send(FdClientVec[i].first, FdClientVec[i].second.response.autoindexbody.c_str(), FdClientVec[i].second.response.autoindexbody.size(), 0)  == -1)
				std::cout << "Autoindex send error" << std::endl;
			FdClientVec[i].second.initialResponseSent = false;
        	FdClientVec[i].second.request.clear();
       		currently_served_quantity--;
			return ;
		}
        FdClientVec[i].second.initialResponseSent = true;
        FdClientVec[i].second.content_length = FdClientVec[i].second.response.contentlength;
    }
    const int chunkSize = 1024;
    char buffer[chunkSize];
    ssize_t bytesRead;
    bytesRead = read(FdClientVec[i].second.file_fd, buffer, chunkSize);
    if (bytesRead > 0){
        std::stringstream chunkSizeHex;
        chunkSizeHex << std::hex << bytesRead << "\r\n";
        std::string chunkSizeHexStr = chunkSizeHex.str();
        std::string chunkData(buffer, bytesRead);
        std::string chunk = chunkSizeHexStr + chunkData + "\r\n";
        ssize_t bytesSent = send(FdClientVec[i].first, chunk.c_str(), chunk.length(), 0);
        if (bytesSent < 0){
            perror("Error sending chunk");
            exit(EXIT_FAILURE);
        }
    }
    else{
        std::string lastChunk = "0\r\n\r\n";
        if (send(FdClientVec[i].first, lastChunk.c_str(), lastChunk.length(), 0) < 0){
            perror("Error sending last chunk");
            exit(EXIT_FAILURE);
        }
        close(FdClientVec[i].second.file_fd);
        close(FdClientVec[i].first);
        FdClientVec[i].second.ResetClient();
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
        for (std::set<int>::iterator it_idx = activeClientIdx.begin(); it_idx != activeClientIdx.end(); it_idx++)
        {
            (void) FdClientVec[*it_idx].second.server_full;
            if (fds[*it_idx].revents & POLLIN){
                server_mapping_request(*it_idx);
				FdClientVec[*it_idx].second.CreateResponse(FdClientVec[*it_idx].second.request, ConfigVec[FdClientVec[*it_idx].second.socket]);
            }
            if (fds[*it_idx].revents & POLLIN &&
            FdClientVec[*it_idx].second.request["method:"].substr(0, 4) == "POST"){
                std::cout << "post is reached\n";
            }
            if (fds[*it_idx].revents & POLLOUT &&
            FdClientVec[*it_idx].second.response.method == "GET")
			{
        		try{
        		    get_request(*it_idx);
        		}
        		catch(const std::exception & e)
        		{
        		    std::cerr << e.what();
        		}
            }
        }
    }
    for (std::set<int>::iterator it_idx = activeClientIdx.begin(); it_idx != activeClientIdx.end(); it_idx++)
    {
        close(FdClientVec[*it_idx].first);
        close(fds[*it_idx].fd);
    }
}

void	HTTP_server::InitFdClientVec()
{
	for (int i = 0; i < (MAX_CLIENTS + listening_port_no + 1); i++)
	{
        Client init;
        std::pair<int, Client> tmp(-1, init);
		FdClientVec.push_back(tmp);
        std::cout << i << std::endl;
	}
}

int HTTP_server::handle_request(std::string path){

	ConfigCheck check;

	listening_port_no = check.checkConfig(path);
    fds = new struct pollfd[MAX_CLIENTS + listening_port_no + 1];
    // FdClientVec = new std::vector<std::pair<int, Client> >[MAX_CLIENTS + listening_port_no + 1];
	InitFdClientVec();

	for (int i = 0; i < listening_port_no; i++)
	{
		ServerConfig tmp(path, i);
		Socket socket(atoi(tmp.port.c_str()), tmp.host);
		FdClientVec[i].first = socket.server_fd;
		ConfigVec.push_back(tmp);
		std::cout << "Socket " << (i + 1) << " (FD " << FdClientVec[i].first
				  << ") is listening on: " << tmp.getConfProps("listen:") << std::endl;
	}
	create_pollfd_struct();
	server_loop();
	return 0;
}