#include "../includes/HTTP_server.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ConfigCheck.hpp"
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <fstream>

#define BUF_SIZE 4096
#define TIMEOUT 20

/**
 * Constructor of HTTP server, creates an array of client
 *
 * @param filename The name of the file to read.
 * @return The contents of the file as a string.
 */
HTTP_server::HTTP_server(){
    timeoutDuration = TIMEOUT;
    // clients = new Client[MAX_CLIENTS];
    // currently_served_quantity = 0;
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
void HTTP_server::tokenizing(std::map<std::string, std::string> & request, std::string line_to_tokenize)
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
                client_len = sizeof(client_addr);
                FdClientVec[j].first = accept(FdClientVec[i].first, (struct sockaddr *)&client_addr, &client_len);
                activeClientIdx.insert(j);
                if (FdClientVec[j].first < 0)
		        {
                    perror("Error accepting client connection on etc");
                    exit(EXIT_FAILURE);
                }
                fds[j].fd = FdClientVec[j].first;
                FdClientVec[j].second.socket = i;
                FdClientVec[j].second.lastInteractionTime = std::time(nullptr);
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

std::map<std::string, std::string> HTTP_server::server_mapping_request(int i){
    std::string key;
    std::map<std::string, std::string> new_request;
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
        std::cout << line << std::endl;
        lines.push_back(strLine);
        line = strtok(NULL, "\n");
    }
    if (!lines.empty()){
        new_request["method:"] = std::strtok(&lines.front()[0], " ");
        new_request["location:"] = std::strtok(NULL, " ");
        new_request["HTTP_version:"] = std::strtok(NULL, " ");
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
            tokenizing(new_request, lines.front());
        }
        lines.pop_front();
    }
    size_t v;
    if ((v = new_request["Content-Type:"].find("boundary=")) != std::string::npos)
    {
        std::string key = new_request["Content-Type:"].substr(v, 8);
        std::string value = new_request["Content-Type:"].substr(new_request["Content-Type:"].find("=") + 1);
        new_request[key] = value;
        new_request["Content-Type:"] = new_request["Content-Type:"].substr(0, new_request["Content-Type:"].find(";"));
    }
    return new_request;
}

void HTTP_server::get_request(int i, std::vector<Request>::iterator req){
    if (!req->initialResponseSent)
	{
		if (req->response.path != "AUTOINDEX")
		{
	        int file_fd = open(req->response.path.c_str(), O_RDONLY);
	        if (file_fd < 0)
			{
	            // close(FdClientVec[i].second.file_fd);
	            // close(FdClientVec[i].first);
	            // FdClientVec[i].second.initialResponseSent = false;
	            // FdClientVec[i].second.request.clear();
	            perror("Error opening file");
	            throw(InvalidFileDownloadException());
	        }
        req->file_fd = file_fd;
		}

        if (send(req->client_fd, req->response.ResponseHeader.c_str(), req->response.ResponseHeader.size(), 0) < 0)
		{
            perror("Error sending initial response headers");
            exit(EXIT_FAILURE);
        }

		if (req->response.path == "AUTOINDEX")
		{
			if (send(req->client_fd, req->response.autoindexbody.c_str(), req->response.autoindexbody.size(), 0)  == -1)
				perror("Autoindex send error");
        	req->requestdone = true;
			return ;
		}
        req->initialResponseSent = true;
    }
    const int chunkSize = BUF_SIZE;
    char buffer[chunkSize];
    ssize_t bytesRead;
    bytesRead = read(req->file_fd, buffer, chunkSize);
    if (bytesRead > 0){
        std::stringstream chunkSizeHex;
        chunkSizeHex << std::hex << bytesRead << "\r\n";
        std::string chunkSizeHexStr = chunkSizeHex.str();
        std::string chunkData(buffer, bytesRead);
        std::string chunk = chunkSizeHexStr + chunkData + "\r\n";
        ssize_t bytesSent = send(req->client_fd, chunk.c_str(), chunk.length(), 0);
        if (bytesSent < 0){
            perror("Error sending chunk");
            exit(EXIT_FAILURE);
        }
    }
    else{
        std::string lastChunk = "0\r\n\r\n";
        if (send(req->client_fd, lastChunk.c_str(), lastChunk.length(), 0) < 0){
            perror("Error sending last chunk");
            exit(EXIT_FAILURE);
        }
        req->requestdone = true;
    }
    FdClientVec[i].second.lastInteractionTime = std::time(nullptr);
}

std::string HTTP_server::toHex(int value)
{
	std::stringstream stream;
	stream << std::hex << value;
	return stream.str();
}

bool    HTTP_server::CheckForTimeout(int i)
{
    currentTime = std::time(nullptr);

    std::time_t elapsedDuration = currentTime - FdClientVec[i].second.lastInteractionTime;
    if (elapsedDuration >= timeoutDuration)
        return true;
    return false;
}

// stashed everything i did, i need to set error codes correctly, set pollout

void HTTP_server::server_loop()
{
    while (true)
    {
        server_conducts_poll();
        for (int i = 0; i < listening_port_no; i++)
                server_port_listening(i);
        for (std::set<int>::iterator it_idx = activeClientIdx.begin(); it_idx != activeClientIdx.end(); it_idx++)
        {
            if (fds[*it_idx].revents & POLLIN)
            {
                Request new_req;
                if (FdClientVec[*it_idx].second.server_full)
                    new_req.GenerateOverloadError(503, ConfigVec[FdClientVec[*it_idx].second.socket]);
                else
                {
                    new_req.request = server_mapping_request(*it_idx);
				    new_req.CreateResponse(ConfigVec[FdClientVec[*it_idx].second.socket]);
                    FdClientVec[*it_idx].second.lastInteractionTime = std::time(nullptr);
                }
                new_req.client_fd = FdClientVec[*it_idx].first;
                FdClientVec[*it_idx].second.RequestVector.push_back(new_req);
                if (new_req.response.cutoffClient)
                    fds[*it_idx].events = POLLOUT;
            }
            if (fds[*it_idx].revents & POLLOUT)
            {
                for (std::vector<Request>::iterator it_req = FdClientVec[*it_idx].second.RequestVector.begin(); it_req != FdClientVec[*it_idx].second.RequestVector.end(); it_req++)
                {
        	    	    try{
        	    	        get_request(*it_idx, it_req);
        	    	    }
        	    	    catch(const std::exception & e)
        	    	    {
        	    	        std::cerr << e.what();
        	    	    }
                    if (it_req->requestdone)
                    {
                        if (it_req->response.cutoffClient)
                            FdClientVec[*it_idx].second.cutoffClient = true;
                        std::cout << "Response: " << it_req->id << " sent to client: " << *it_idx << std::endl;
                        FdClientVec[*it_idx].second.RequestVector.erase(it_req);
                        break ;
                    }
                }
            }
            if (FdClientVec[*it_idx].second.cutoffClient || (FdClientVec[*it_idx].second.RequestVector.empty() && (CheckForTimeout(*it_idx))))
            {
                close(fds[*it_idx].fd);
                fds[*it_idx].events = POLLIN | POLLOUT;
                fds[*it_idx].fd = -1;
                FdClientVec[*it_idx].second.ResetClient();
                std::cout << "Connection Timeout - Client " << *it_idx << " disconnected" << std::endl;
                activeClientIdx.erase(it_idx);
                break;
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
	}
}

int HTTP_server::handle_request(std::string path){

	ConfigCheck check;

	listening_port_no = check.checkConfig(path);
    fds = new struct pollfd[MAX_CLIENTS + listening_port_no + 1];
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