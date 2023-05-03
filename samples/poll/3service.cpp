//example

void listen_for_connections( int service1_sock, int service2_sock, int service3_sock)
{
    struct pollfd pollfds [3];
    pollfds[0].fd = service1_sock;
    pollfds[0].events = POLLIN;
    pollfds[1].fd = service2_sock;
    pollfds[1].events = POLLIN;
    pollfds[2].fd = service3_sock;
    pollfds[2].events = POLLIN;

    int timeout = 30 * 1000; /* 30 seconds in ms */
    printf( "Going to start listening for socket events.\n");
}

while ( !quit)
{
    int res = poll (&pollfds, 3, timeout);
    if (res == -1){ /* An error has occured */
        printf("An error occured in select(): %s.\n", strerror( errno ));
        quit = 1;
    } else if (res == 0){/* 0 sockets had events occur */
        printf( "Still waiting; nothing occured recently. \n");
    }
    else{ /*Things happened*/
        if (pollfds[0].ravents & POLLIN){
            service1_activate();
        }
        if (pollfds[1].ravents & POLLIN){
            service2_activate();
        }
        if (pollfds[2].ravents & POLLIN){
            service3_activate();
        }
    }
}