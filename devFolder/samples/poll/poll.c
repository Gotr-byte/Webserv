#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

int main()
{
    char name[255];
    int counter = 0;
    
    struct pollfd mypoll;
    //lets init mypoll
    memset(&mypoll, 0, sizeof(mypoll));
    mypoll.fd = 0;
    mypoll.events = POLLIN;
    printf("Type in your name:\n");
    while(1)
    {
        poll(&mypoll, 1, 100);
        if(mypoll.revents &POLLIN){
            read(0, name, sizeof(name));
            printf("Hello, %s", name);
            break;
        }
        else
            counter++;
    }
    printf("It took you %d ms to type in your name\n", 100*counter);
    return 0;
}