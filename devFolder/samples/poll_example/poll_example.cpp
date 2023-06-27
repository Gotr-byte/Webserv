#include <stdio.h>
#include <poll.h>
#include <vector>

int main(void)
{
    std::vector<pollfd> pfds(1); // Create a vector with one element
    pfds[0].fd = 0;              // Standard input
    pfds[0].events = POLLIN;     // Tell me when ready to read

    printf("Hit RETURN or wait 2.5 seconds for timeout\n");
    int num_events = poll(&pfds[0], 1, 2500); // 2.5 second timeout

    if (num_events == 0) {
        printf("Poll timed out!\n");
    } else {
        int pollin_happened = pfds[0].revents & POLLIN;

        if (pollin_happened) {
            printf("File descriptor %d is ready to read\n", pfds[0].fd);
        } else {
            printf("Unexpected event occurred: %d\n", pfds[0].revents);
        }
    }

    return 0;
}
