#include "../includes/RequestHandler.hpp"
#include <cstddef>
#include <cstdlib>
#include <limits>

//TODO add usleep here for performance?
//TODO closing client sockets that disconected?
//TODO check request validity
//TODO work on the structure of the poll request


#define BUF_SIZE 4096
#define LISTENING_SOCKETS_NUMBER 2
#define ETC_PORT 4007
#define BUFSIZE 1024

std::string RequestHandler::read_file(const std::string& filename)
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

void RequestHandler::tokenizing( std::map<std::string, std::string>& request, std::string line_to_tokenize)
{
    std::stringstream   tokenize_stream(line_to_tokenize);
    std::string         value;
    std::string         key;
    std::getline(tokenize_stream, key, ' ');
    std::getline(tokenize_stream, value, ' ');
    request[key] = value;
}

void RequestHandler::place_in_file(std::string line_to_file) {
    FILE* fp = fopen("received_file", "wb");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open the file.\n");
        return;
    }

    size_t length = strlen(line_to_file.c_str());
    std::cout << length << "\n";
    // size_t bytesWritten = fwrite(line_to_file.c_str(), 1, length, fp);
    // if (bytesWritten != length) {
    //     fprintf(stderr, "Failed to write the entire string to the file.\n");
    // }

    fclose(fp);
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

void RequestHandler::print_map()
{
    std::map<std::string, std::string>::const_iterator it;
    for (it = request.begin(); it != request.end(); ++it) {
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
    }
}

void RequestHandler::server_loop()
{
    // Start the main loop
    int number_of_clients = 0;
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
            fds[clients.size() + LISTENING_SOCKETS_NUMBER - 1].events = POLLIN|POLLOUT;
            fds[clients.size() + LISTENING_SOCKETS_NUMBER - 1].revents = 0;

            char buffer[BUFSIZE] = {0};
            read(client_fd, buffer, BUFSIZE);
            // char path[BUFSIZE] = {0};
            // sscanf(buffer, "GET %s", path);
            // Here we can actually state wich methods are granted use from this perticular listening port
            std::cout << buffer << "\n";
            std::cout << "New client connected on etc fd\n";
        }
        // Check for events on any of the connected client sockets
        for (unsigned long i = 0; i < clients.size(); i++) 
        {
            // printf("i equals %lu\n",i);
            if (fds[i + LISTENING_SOCKETS_NUMBER].revents & POLLIN) 
            {
                std::cout <<"Number of poll clients"<< number_of_clients << "\n";
                number_of_clients++;
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
                while (line != NULL)
                {
                    std::string strLine(line);
                    std::cout << strLine << "\n";
                    lines.push_back(strLine);
                    line = strtok(NULL, "\n");
                }
                if(!lines.empty())
                    request["method:"] = std::strtok(&lines.front()[0], " ");
                if(!lines.empty())
                    request["location:"] = std::strtok(NULL, " ");
                if(!lines.empty())
                    request["HTTP_version:"] = std::strtok(NULL, " ");
                int new_line_count = 0;
                while(!lines.empty())
                {
                    if(!lines.empty())
                    {
                        if(lines.front() == "\n" && new_line_count == 1)
                        {
                            lines.pop_front();
                            break;
                        }
                        if(lines.front() == "\r")
                        {
                            new_line_count++;
                        }
                        else
                        {
                            tokenizing(request, lines.front());
                        }
                    }
                    lines.pop_front();
                }
                print_map();

                // if(request["method:"].substr(0,4) == "POST")
                // {
                //     std::cout << "post is reached" << "\n";
                //     while(!lines.empty())
                //     {
                //         std::cout << lines.front() << "\n";
                //         lines.pop_front();
                //     }
                //     // place_in_file(lines.front().c_str());
                //     const char* httpFileSentResponse =
                //                         "HTTP/1.1 200 OK\r\n"
                //                         "Content-Type: text/html\r\n"
                //                         "Content-Length: 135\r\n"
                //                         "\r\n"
                //                         "<!DOCTYPE html>\r\n"
                //                         "<html>\r\n"
                //                         "<head>\r\n"
                //                         "    <title>File Upload</title>\r\n"
                //                         "</head>\r\n"
                //                         "<body>\r\n"
                //                         "    <h1>File Upload Successful</h1>\r\n"
                //                         "    <p>Your file has been successfully uploaded to the server.</p>\r\n"
                //                         "</body>\r\n"
                //                         "</html>\r\n";
                //     int s = send(clients[i], httpFileSentResponse, strlen(httpFileSentResponse), 0);
                //     if (s < 0)
                //     {
                //         perror("Error sending data to client");
                //         exit(EXIT_FAILURE);
                //     }
                //     exit(EXIT_SUCCESS);
                // }
                if(request["method:"].substr(0,3) == "GET")
                {
                    if(request["location:"].substr(0, 6) =="/file/")
                    {
                        filename = ".." + request["location:"];
                        std::cout << filename << "\n";
                        content = read_file(filename);
                        std::stringstream int_to_string;
                        int_to_string << content.length();
                        std::string content_length = int_to_string.str();
                        http_response = "HTTP/1.1 200 OK\nContent-Length:" + content_length;
                        http_response = http_response + "\n\n";
                        message = http_response + content;
                        int s = send(clients[i], message.c_str(), message.length(), 0);
                        if (s < 0)
                        {
                            perror("Error sending data to client");
                            exit(EXIT_FAILURE);
                        }    
                    }
                    else if((request["location:"].substr(0, 6) == "/HTML/"))
                    {
                        filename = ".." + request["location:"];
                        std::cout << filename << "\n";
                        content = read_file(filename);
                        std::stringstream int_to_string;
                        int_to_string << content.length();
                        std::string content_length = int_to_string.str();
                        http_response = "HTTP/1.1 200 OK\nContent-Length:" + content_length;
                        http_response = http_response + "\n\n";
                        message = http_response + content;
                        int s = send(clients[i], message.c_str(), message.length(), 0);
                        if (s < 0)
                        {
                            perror("Error sending data to client");
                            exit(EXIT_FAILURE);
                        }    
                    }
                    if(strcmp(request["location:"].c_str(), "/cgi-bin/create_file.py") == 0)
                    {
                        pid_t pid = fork();
                        if (pid == 0)
                        {
                            // Child process
                            // Execute the Python script using exec
                            std::string cgi_to_run =  ".." + request["location:"];
                            std::cout << cgi_to_run << "\n";
                            execlp("python", "python", cgi_to_run.c_str(), NULL);
                            
                            perror("exec");
                            exit(1);
                        }
                        else if (pid > 0)
                        {
                            // Parent process
                            
                            // Wait for the child process to finish
                            int status;
                            waitpid(pid, &status, 0);
                            
                            // Check the exit status of the child process
                            if (WIFEXITED(status))
                            {
                                int exit_status = WEXITSTATUS(status);
                                printf("Child process exited with status: %d\n", exit_status);
                            }
                            else
                            {
                                printf("Child process terminated abnormally.\n");
                            }
                        }
                        else
                        {
                            // Fork failed
                            perror("fork");
                            exit(1);
                        }
                    }
                    if(strcmp(request["location:"].c_str(), "/cgi-bin/remove_file.py") == 0)
                    {
                        pid_t pid = fork();
                        if (pid == 0)
                        {
                            // Child process
                            // Execute the Python script using exec
                            std::string cgi_to_run =  ".." + request["location:"];
                            std::cout << cgi_to_run << "\n";
                            execlp("python3", "python3", cgi_to_run.c_str(), NULL);
                            
                            // If exec returns, an error occurred
                            perror("exec");
                            exit(1);
                        }
                        else if (pid > 0)
                        {
                            // Parent process
                            
                            // Wait for the child process to finish
                            int status;
                            waitpid(pid, &status, 0);
                            
                            // Check the exit status of the child process
                            if (WIFEXITED(status))
                            {
                                int exit_status = WEXITSTATUS(status);
                                printf("Child process exited with status: %d\n", exit_status);
                            }
                            else
                            {
                                printf("Child process terminated abnormally.\n");
                            }
                        }
                        else
                        {
                            // Fork failed
                            perror("fork");
                            exit(1);
                        }
                    }
                }
              
                // close(clients[i]);
                request.clear();
                lines.clear();
                // sleep(1);
            }
        }

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
