// // Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

// #define PORT 8080
// int main(int argc, char const *argv[])
// {
//     int server_fd, new_socket; long valread;
//     struct sockaddr_in address;
//     int addrlen = sizeof(address);
    
//     // Only this line has been changed. Everything is same.
//     // TODO send page here
//     // TODO send multiple pages
//     // TODO send file here through link
//     // TODO config file
//     // char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
//     char *hello = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<!DOCTYPE html>\n<html>\n<head>\n<title>My Static Site</title>\n</head>\n<body>\n<h1>Welcome to my static site!</h1>\n<p>This is a simple HTML site.</p>\n<p>You can add more content here, such as images, links, and additional HTML elements.</p>\n</body>\n</html>";
//     // Creating socket file descriptor
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//     {
//         perror("In socket");
//         exit(EXIT_FAILURE);
//     }
    

//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons( PORT );
    
//     memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
//     {
//         perror("In bind");
//         exit(EXIT_FAILURE);
//     }
//     if (listen(server_fd, 10) < 0)
//     {
//         perror("In listen");
//         exit(EXIT_FAILURE);
//     }
//     while(1)
//     {
//         printf("\n+++++++ Waiting for new connection ++++++++\n\n");
//         if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
//         {
//             perror("In accept");
//             exit(EXIT_FAILURE);
//         }
        
//         char buffer[30000] = {0};
//         valread = read( new_socket , buffer, 30000);
//         printf("%s\n",buffer );
//         write(new_socket , hello , strlen(hello));
//         printf("------------------Hello message sent-------------------");
//         close(new_socket);
//     }
//     return 0;
// }


// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Only this line has been changed. Everything is same.
    // TODO send page here
    // TODO send multiple pages
    // TODO send file here through link
    // TODO config file
    // char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    // char *hello = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<!DOCTYPE html>\n<html>\n<head>\n<title>My Static Site</title>\n</head>\n<body>\n<h1>Welcome to my static site!</h1>\n<p>This is a simple HTML site.</p>\n<p>You can add more content here, such as images, links, and additional HTML elements.</p>\n</body>\n</html>";
    char *http_response = "HTTP/1.1 200 OK\r\n"
                      "Server: nginx/1.20.1\r\n"
                      "Date: Mon, 02 May 2023 00:00:00 GMT\r\n"
                      "Content-Type: text/plain\r\n"
                      "Content-Length: 5120\r\n"
                      "Last-Modified: Sun, 01 May 2023 12:00:00 GMT\r\n"
                      "Connection: keep-alive\r\n"
                      "ETag: \"1234567890\"\r\n"
                      "Accept-Ranges: bytes\r\n\r\n";
    open()
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        buffer = read 
        // valread = read( new_socket , buffer, 30000);
        // printf("%s\n",buffer );
        write(new_socket , http_response , strlen(http_response));
        // printf("------------------Hello message sent-------------------");
        close(new_socket);
    }
    return 0;
}


