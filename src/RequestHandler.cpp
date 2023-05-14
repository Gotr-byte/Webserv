#include "../includes/RequestHandler.hpp"
#include <cstddef>

#define BUF_SIZE 4096
#define LISTENING_SOCKETS_NUMBER 2
#define ETC_PORT 4008
#define BUFSIZE 1024

static std::string read_file(const std::string& filename)
{
    std::ifstream file(filename.c_str());

    if (!file) {
        std::cerr << "Error opening file " << filename << std::endl;
        return "";
    }

    std::string content;
    char c;

    while (file.get(c)) {
        content += c;
    }

    file.close();

    return content;
}

// void tokenizing(std::deque<std::string>& lines)
void tokenizing( std::map<std::string, std::string>& request, std::string line_to_tokenize)
{
    std::stringstream   tokenize_stream(line_to_tokenize);
    std::string         value;
    std::string         key;
    std::getline(tokenize_stream, key, ' ');
    std::getline(tokenize_stream, value, ' ');
    request[key] = value;
}

void RequestHandler::create_listening_sock(int port)
{
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    // set the socket to non blocking
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    // Set the server socket options
    opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Error setting server socket options");
        exit(EXIT_FAILURE);
    }


    // Bind the server socket to a specific port
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding server socket port");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections on the server socket
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Error listening on server socket");
        exit(EXIT_FAILURE);
    }
    listening_socket_fd.push_back(server_fd);
}

void RequestHandler::create_pollfd_struct(void)
{
    memset(fds, 0, sizeof(fds));
    fds[0].fd = listening_socket_fd[0];
    fds[0].events = POLLIN|POLLOUT;
    fds[1].fd = listening_socket_fd[1];
    fds[1].events = POLLIN|POLLOUT;
}

void RequestHandler::server_loop()
{
    // Start the main loop
    while (true)
    {

        // Wait for events on any of the sockets
        nfds = clients.size() + LISTENING_SOCKETS_NUMBER;
        res = poll(fds, nfds, 200);
        if (res < 0) {
            perror("Error polling sockets");
            exit(EXIT_FAILURE);
        }
        // Check for events on the server socket
        if (fds[0].revents & POLLIN) {

            // Accept a new connection from a client
            client_len = sizeof(client_addr);
            int client_fd = accept(listening_socket_fd[0], (struct sockaddr *)&client_addr, &client_len);
            if (client_fd < 0) {
                perror("Error accepting client connection on addr");
                exit(EXIT_FAILURE);
            }

            // Add the client socket to the list of connected clients
            clients.push_back(client_fd);

            // Initialize the pollfd struct for the client socket
            fds[clients.size() + LISTENING_SOCKETS_NUMBER - 1].fd = client_fd;
            fds[clients.size() + LISTENING_SOCKETS_NUMBER - 1].events = POLLIN|POLLOUT;
            fds[clients.size() + LISTENING_SOCKETS_NUMBER - 1].revents = 0;

            std::cout << "New client connected on server fd\n";
        }
        if (fds[1].revents & POLLIN)
        {
            // Accept a new connection from a client
            client_len = sizeof(client_addr);
            int client_fd = accept(listening_socket_fd[1], (struct sockaddr *)&client_addr, &client_len);
            if (client_fd < 0)
            {
                perror("Error accepting client connection on etc");
                exit(EXIT_FAILURE);
            }

            // Add the client socket to the list of connected clients
            clients.push_back(client_fd);

            // Initialize the pollfd struct for the client socket
            fds[clients.size() + LISTENING_SOCKETS_NUMBER - 1].fd = client_fd;
            fds[clients.size() + LISTENING_SOCKETS_NUMBER - 1].events = POLLOUT;
            fds[clients.size() + LISTENING_SOCKETS_NUMBER - 1].revents = 0;

            char buffer[BUFSIZE] = {0};
            read(client_fd, buffer, BUFSIZE);
            char path[BUFSIZE] = {0};
            sscanf(buffer, "GET %s", path);
            std::cout << "New client connected on etc fd\n";
        }
        // Check for events on any of the connected client sockets
        for (unsigned long i = 0; i < clients.size(); i++) 
        {
            // printf("i equals %lu\n",i);
            if (fds[i + LISTENING_SOCKETS_NUMBER].revents & POLLIN) 
            {
                std::string key;
                // Receive data from the client
                char buf[BUF_SIZE];
                memset(buf, 0, BUF_SIZE);
                int n = recv(clients[i], buf, BUF_SIZE, 0);
                if (n < 0)
                {
                    perror("Error receiving data from client");
                    exit(EXIT_FAILURE);
                }
                std::string HTTP_request(buf);
                line = std::strtok(&HTTP_request[0], "\n");
                while (line != NULL) {
                    std::string strLine(line);
                    lines.push_back(strLine);
                    line = strtok(NULL, "\n");
                }
                if(!lines.empty())
                    request["method:"] = std::strtok(&lines.front()[0], " ");
                if(!lines.empty())
                    request["location:"] = std::strtok(NULL, " ");
                if(!lines.empty())
                    request["HTTP_version:"] = std::strtok(NULL, " ");
                while(!lines.empty())
                {
                    if(!lines.empty())
                        tokenizing(request, lines.front());
                    lines.pop_front();
                }
                //TODO check request validity
                // if(request["location:"] == "../HTML/")
                std::cout << ".." << request["location:"] << "\n";
                std::map<std::string, std::string>::iterator it = request.begin();
                for (it = request.begin(); it != request.end(); it++) {
                    std::cout << "Key: " << it->first << ", Value: " << it->second <<std::endl;
                }
                //buf is our whole request
                std::cout << request["location:"] << "\n";
                std::cout << "LOOK BELOW\n";
                std::cout << request["location:"].substr(0, 6) << "\n";
                if (request["location:"].substr(0, 6) =="/file/")
                {
                    content_type = " text/txt";
                    std::cout << "WE FOUND THE files folder SIRE!\n"; 

                }
                if (request["location:"].substr(0, 6) == "/HTML/")
                {
                    content_type = " text/html";
                    std::cout << "WE FOUND THE HTML SIRE!\n"; 
                }
                filename = ".." + request["location:"];
                std::cout << filename << "\n";
                content = read_file(filename);
                std::stringstream int_to_string;
                int_to_string << content.length();
                std::string content_length = int_to_string.str();
                //the content type is dependant on the folder
                http_response = "HTTP/1.1 200 OK\nContent-Type:" + content_type + "\nContent-Length:" + content_length;
                http_response = http_response + "\n\n";
                message = http_response + content;
                int s = send(clients[i], message.c_str(), message.length(), 0);
                if (s < 0)
                {
                    perror("Error sending data to client");
                    exit(EXIT_FAILURE);
                }
                request.clear();
                lines.clear();
                sleep(1);
            }
        }
        //TODO add usleep here for performance?
        //TODO closing client sockets that disconected?
    }
    // Close all connected client sockets
    for (unsigned long i = 0; i < clients.size(); i++) {
        close(clients[i]);
    }
    // Close the server socket
    std::vector<int>::iterator it;
    for (it = listening_socket_fd.begin(); it != listening_socket_fd.end(); it++)
    {
        close(*it);
    }
}

int RequestHandler::handleRequest(int port) {
    create_listening_sock(port);
    create_listening_sock(ETC_PORT);
    create_pollfd_struct();
    server_loop();
    return 0;
}
