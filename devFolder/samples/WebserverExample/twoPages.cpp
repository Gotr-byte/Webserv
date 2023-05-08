#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n"
                  "<html><body><h1>Hello, World!</h1></body></html>";
    
    char *otherPage = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n"
                      "<html><body><h1>This is another page!</h1></body></html>";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    while (true) {
        std::cout << "Waiting for new connection..." << std::endl;

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            return -1;
        }

        char buffer[30000] = {0};
        valread = read(new_socket, buffer, 30000);

        std::cout << "Received request:\n" << buffer << std::endl;

        std::string response = "";

        if (strncmp(buffer, "GET / ", 6) == 0 || strncmp(buffer, "GET /index.html", 15) == 0) {
            response = hello;
        } else if (strncmp(buffer, "GET /other.html", 15) == 0) {
            response = otherPage;
        } else {
            response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n"
                       "<html><body><h1>404 Not Found</h1></body></html>";
        }

        send(new_socket, response.c_str(), response.length(), 0);

        std::cout << "Response sent:\n" << response << std::endl;

        close(new_socket);
    }

    return 0;
}