#include "../includes/HTTP_server.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ConfigCheck.hpp"
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <fstream>

//TODO closing client sockets that disconected?
//TODO check request validity
//TODO work on the structure of the poll request
//Notes - should the requests be limited to bufsize 1024?

#define BUF_SIZE 1024

std::string HTTP_server::read_file(const std::string& filename){
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

void HTTP_server::tokenizing( std::map<std::string, std::string>& request, std::string line_to_tokenize){
    std::stringstream   tokenize_stream(line_to_tokenize);
    std::string         value;
    std::string         key;
    std::getline(tokenize_stream, key, ':');
    std::getline(tokenize_stream, value, ':');
    request[key] = value;
}

void HTTP_server::create_listening_sock(int port){
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    // set the socket to non blocking
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Error setting server socket options");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
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

void HTTP_server::create_pollfd_struct(void){
    memset(fds, 0, sizeof(fds));
    for (int i = 0; i < listening_port_no; i++)
    {
        fds[i].fd = listening_socket_fd[i];
        fds[i].events = POLLIN|POLLOUT;
    }
}

// void HTTP_server::print_map(){
//     std::map<std::string, std::string>::const_iterator it;
//     for (it = request.begin(); it != request.end(); ++it) {
//         std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
//     }
// }

void HTTP_server::server_port_listening(int client_fd, int i){
    if (fds[i].revents & POLLIN)
    {
        // Accept a new connection from a client
        client_len = sizeof(client_addr);
        client_fd = accept(listening_socket_fd[i], (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            perror("Error accepting client connection on etc");
            exit(EXIT_FAILURE);
        }

        // Add the client socket to the list of connected clients
        clients.push_back(client_fd);

        // Initialize the pollfd struct for the client socket
        fds[clients.size() + listening_port_no - 1].fd = client_fd;
        fds[clients.size() + listening_port_no - 1].events = POLLIN|POLLOUT;
        fds[clients.size() + listening_port_no - 1].revents = 0;

        std::cout << "New client connected on etc fd\n";
    }
}

void HTTP_server::server_conducts_poll(){
    nfds = clients.size() + listening_port_no;
    res = poll(fds, nfds, 10);
    if (res < 0)
    {
        perror("Error polling sockets");
        exit(EXIT_FAILURE);
    }
}

void HTTP_server::server_mapping_request(int i){
    std::string key;
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
        // std::cout << strLine << "\n";
        lines.push_back(strLine);
        line = strtok(NULL, "\n");
    }
    if(!lines.empty())
        request[i]["method:"] = std::strtok(&lines.front()[0], " ");
    if(!lines.empty())
        request[i]["location:"] = std::strtok(NULL, " ");
    if(!lines.empty())
        request[i]["HTTP_version:"] = std::strtok(NULL, " ");
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
                tokenizing(request[i], lines.front());
            }
        }
        lines.pop_front();
    }
}

void HTTP_server::perform_get_request(int i)
{
    if(request[i]["location:"].substr(0, 6) =="/file/")
    {
        filename = ".." + request[i]["location:"];
        std::cout << filename << "\n";
        content = read_file(filename);
        std::stringstream int_to_string;
        int_to_string << content.length();
        std::string content_length = int_to_string.str();
        http_response = "HTTP/1.1 200 OK\nContent-Length:" + content_length;
        http_response = http_response + "\n\n";
        message = http_response + content;
        sentBytes[i] = send(clients[i], message.c_str(), content.length(), 0);
        if (sentBytes[i] < 0)
        {
            perror("Error sending data to client");
            exit(EXIT_FAILURE);
        }
        if (sentBytes[i] ==  0)
        {
            close(clients[i]);
            clients.erase(clients.begin() + i);
            request.erase(i);
        }
        // close(clients[i]);
        clients.erase(clients.begin() + i);
    }
    else if((request[i]["location:"].substr(0, 6) == "/HTML/"))
    {
        filename = ".." + request[i]["location:"];
        // std::cout << filename << "\n";
        //todo enter buffer here
        content = read_file(filename);
        std::stringstream int_to_string;
        int_to_string << content.length();
        std::string content_length = int_to_string.str();
        http_response = "HTTP/1.1 200 OK\nContent-Length:" + content_length;
        http_response = http_response + "\n\n";
        message = http_response + content;
        sentBytes[i] = send(clients[i], message.c_str(), content.length(), 0);
        if (sentBytes[i] < 0)
        {
            perror("Error sending data to client");
            exit(EXIT_FAILURE);
        }
        if (sentBytes[i] ==  0)
        {
            close(clients[i]);
            clients.erase(clients.begin() + i);
            request.erase(i);
        }

    }
    else if(strcmp(request[i]["location:"].c_str(), "/cgi-bin/create_file.py") == 0)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            std::string cgi_to_run =  ".." + request[i]["location:"];
            std::cout << cgi_to_run << "\n";
            execlp("python", "python", cgi_to_run.c_str(), NULL);
            
            perror("exec");
            exit(1);
        }
        else if (pid > 0)
        {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
            {
                int exit_status = WEXITSTATUS(status);
                printf("Child process exited with status: %d\n", exit_status);
            }
            else
            {
                printf("Child process terminated abnormally.\n");
            }
            close(clients[i]);
            clients.erase(clients.begin() + i);
        }
        else
        {
            perror("fork");
            exit(1);
        }
        
    }
        if(strcmp(request[i]["location:"].c_str(), "/cgi-bin/remove_file.py") == 0)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                std::string cgi_to_run =  ".." + request[i]["location:"];
                std::cout << cgi_to_run << "\n";
                execlp("python3", "python3", cgi_to_run.c_str(), NULL);
                perror("exec");
                exit(1);
            }
            else if (pid > 0)
            {
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status))
                {
                    int exit_status = WEXITSTATUS(status);
                    printf("Child process exited with status: %d\n", exit_status);
                }
                else
                {
                    printf("Child process terminated abnormally.\n");
                }
                close(clients[i]);
                clients.erase(clients.begin() + i);
            }
            else
            {
                perror("fork");
                exit(1);
            }
    }
}

void HTTP_server::server_loop()
{
    int m = 0;
    while (true)
    {
        server_conducts_poll();
        for (int i = 0; i < listening_port_no; i++)
            server_port_listening(listening_socket_fd[i], i);
        for (unsigned long i = 0; i < clients.size(); i++) 
        {
            if (fds[i + listening_port_no].revents & POLLIN)
            {
                server_mapping_request(i);
                if(request[i]["method:"].substr(0,4) == "POST")
                {
                    std::cout << "post is reached" << "\n";

                    // print_map();
                    if (m < 0)
                    {
                        exit(EXIT_FAILURE);
                    }
                    while(!lines.empty())
                    {
                        std::cout << lines.front() << "\n";
                        lines.pop_front();
                    }
                    const char* httpFileSentResponse =
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
                    int s = send(clients[i], httpFileSentResponse, strlen(httpFileSentResponse), 0);
                    if (s < 0)
                    {
                        perror("Error sending data to client");
                        exit(EXIT_FAILURE);
                    }
                    close(clients[i]);
                    clients.erase(clients.begin() + i);
                }
            }
            if(fds[i + listening_port_no].revents & POLLOUT)
            {
                if(request[i]["method:"].substr(0,3) == "GET")
                    perform_get_request(i);
            }
            request.clear();
            lines.clear();
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

int HTTP_server::handleRequest(std::string path) {

    ConfigCheck check;

    listening_port_no = check.checkConfig(path);

    for (int i = 0; i < listening_port_no; i++)
    {
        ServerConfig tmp(path, i);
        configVec.push_back(tmp);
        create_listening_sock(atoi(configVec[i].getConfProps("port:").c_str()));
        std::cout << " Socket "<< (i + 1) << " (FD " << listening_socket_fd[i] \
            << ") is listening on port: " << configVec[i].getConfProps("port:") << std::endl;
    }
    create_pollfd_struct();
    server_loop();
    return 0;
}