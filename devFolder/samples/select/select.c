//socket server using select concept code not working

void listen_for_connection(int service1_sock, int service2_sock, int service3_sock)
{
    int nfds = 1 + (service1_sock > service2_sock
    ? service1_sock > service3_sock ? service1_sock : service3_sock
    : service2_sock > service3_sock ? service2_sock : service3_sock);
    fd_set s;
    struct timeval tv;
    printf("going to start listening for socket events\n");
}

while (!quit)
{
    FD_ZERO( &s );
    FD_SET( service1_sock, &s);
    FD_SET( service2_sock, &s);
    FD_SET( service3_sock, &s);
    tv.tv_sec = 30;
    tv.tv_usec = 0;

    int res = select(nfds, &s, NULL, NULL, &tv);
    if (res == -1){
        printf("Error occured in select: %s. \n", strerror(errno));
        quit = 1;
    }
    else if( res == 0)
    {
        printf("Still waiting: nothing occured recently\n");
    }
    else{
        if (FD_ISSET(service1_sock, &s)){
            service1_activate();
        }
         if (FD_ISSET(service2_sock, &s)){
            service2_activate();
        }
         if (FD_ISSET(service3_sock, &s)){
            service3_activate();
        }
    }
}