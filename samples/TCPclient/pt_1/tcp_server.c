#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

int main()
{
    char server_message[256] = "You have reached the server\n";


    // create the server socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket to our specified IP and port
    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    // the second number is the number of connections you can listen to
    listen(server_socket, 5);

    int client_socket;
    // the second parameter is to fill the socket the information where the data is coming from
    client_socket = accept(server_socket, NULL, NULL);

    //option flag parameter
    send(client_socket, server_message, sizeof(server_message), 0);




    return(0);
}