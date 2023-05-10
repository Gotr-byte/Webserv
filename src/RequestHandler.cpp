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

int RequestHandler::handleRequest(int port) {

    // Create a socket for the server
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

    // int etc_fd = create_socket(etc_port, &server_addr);
   etc_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (etc_fd < 0) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    //set the socket to non blocking
    fcntl(etc_fd, F_SETFL, O_NONBLOCK);

    // Set the server socket options
    int etc_opt = 2;
    if (setsockopt(etc_fd, SOL_SOCKET, SO_REUSEADDR, &etc_opt, sizeof(etc_opt))) {
        perror("Error setting server socket options");
        exit(EXIT_FAILURE);
    }

    // Bind the server socket to a specific port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr_etc.sin_family = AF_INET;
    server_addr_etc.sin_addr.s_addr = INADDR_ANY;
    server_addr_etc.sin_port = htons(ETC_PORT);
    if (bind(etc_fd, (struct sockaddr *)&server_addr_etc, sizeof(server_addr_etc)) < 0) {
        perror("Error binding server socket etc");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections on the server socket
    if (listen(etc_fd, MAX_CLIENTS) < 0) {
        perror("Error listening on server socket");
        exit(EXIT_FAILURE);
    }

    // Initialize the pollfd struct for the server socket
    memset(fds, 0, sizeof(fds));
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    fds[1].fd = etc_fd;
    fds[1].events = POLLIN;

    // Initialize the list of connected client sockets

    // Start the main loop
    while (true)
    {

        // Wait for events on any of the sockets
        int nfds = clients.size() + LISTENING_SOCKETS_NUMBER;
        int res = poll(fds, nfds, 200);
        if (res < 0) {
            perror("Error polling sockets");
            exit(EXIT_FAILURE);
        }
        // Check for events on the server socket
        if (fds[0].revents & POLLIN) {

            // Accept a new connection from a client
            client_len = sizeof(client_addr);
            int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
            if (client_fd < 0) {
                perror("Error accepting client connection on addr");
                exit(EXIT_FAILURE);
            }

            // Add the client socket to the list of connected clients
            clients.push_back(client_fd);

            // Initialize the pollfd struct for the client socket
            fds[clients.size() + LISTENING_SOCKETS_NUMBER - 1].fd = client_fd;
            fds[clients.size() + LISTENING_SOCKETS_NUMBER - 1].events = POLLIN;
            fds[clients.size() + LISTENING_SOCKETS_NUMBER - 1].revents = 0;

            std::cout << "New client connected on server fd\n";
        }
        if (fds[1].revents & POLLIN)
        {
            // Accept a new connection from a client
            client_len = sizeof(client_addr);
            int client_fd = accept(etc_fd, (struct sockaddr *)&client_addr, &client_len);
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
                // HTTP_requests.find()
                // char *line = strtok(&HTTP_request[0], "\n");
                line = std::strtok(&HTTP_request[0], "\n");
                // request["method"] = get_method(&line);
                while (line != NULL) {
                    std::string strLine(line);
                    lines.push_back(strLine);
                    line = strtok(NULL, "\n");
                }
                // std::cout << line;
                if(!lines.empty())
                    request["method:"] = std::strtok(&lines.front()[0], " ");
                if(!lines.empty())
                    request["location:"] = std::strtok(NULL, " ");
                if(!lines.empty())
                    request["HTTP_version:"] = std::strtok(NULL, " ");
                if(!lines.empty())
                    lines.pop_front();
                while(!lines.empty())
                {
                    if(!lines.empty())
                        tokenizing(request, lines.front());
                    lines.pop_front();
                }
                // std::map<std::string, std::string>::iterator it = request.begin();
                // for (it = request.begin(); it != request.end(); it++) {
                //     std::cout << "Key: " << it->first << ", Value: " << it->second <<std::endl;
                // }
                //buf is our whole request
                request.clear();
                lines.clear();
                sleep(1);
                
            }
            if (fds[i + LISTENING_SOCKETS_NUMBER].revents & POLLOUT)
            {
            //Send data to client
                http_response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 159\n\n";
                filename = "../HTML/prototype.html";
                content = read_file(filename);
                message = http_response + content;
                int s = send(clients[i], message.c_str(), message.length(), 0);
                if (s < 0)
                {
                    perror("Error sending data to client");
                    exit(EXIT_FAILURE);
                }

                sleep(10); // this is temporary
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
    close(server_fd);

    return 0;
}
