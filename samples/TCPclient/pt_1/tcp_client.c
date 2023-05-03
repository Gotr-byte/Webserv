#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netinet/in.h>

int main()
{
    //create a socket
    int network_socket;

    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    //specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    if (connection_status < 0)
    {
        printf ("Error: connection\n");
    }
    // recieve data from the server
    char server_response[256];
    recv(network_socket, &server_response, sizeof(server_response), 0);
    //print data that we got from the server
    printf("Data recieved form the server %s \n", server_response);

    //and then close the socket
    close(network_socket);

    return 0;
}
