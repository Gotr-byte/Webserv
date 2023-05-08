#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>

int main() {
    // Create server sockets and bind them to ports
    int server1_fd = socket(AF_INET, SOCK_STREAM, 0);
    // ...
    int server2_fd = socket(AF_INET, SOCK_STREAM, 0);
    // ...

    sockaddr_in addr1, addr2;
    addr1.sin_family = AF_INET;
    addr1.sin_addr.s_addr = INADDR_ANY;
    addr1.sin_port = htons(8080);

    addr2.sin_family = AF_INET;
    addr2.sin_addr.s_addr = INADDR_ANY;
    addr2.sin_port = htons(8081);

    bind(server1_fd, (sockaddr*)&addr1, sizeof(addr1));
    // ...
    bind(server2_fd, (sockaddr*)&addr2, sizeof(addr2));

    // Set the sockets to non-blocking mode
    fcntl(server1_fd, F_SETFL, O_NONBLOCK);
    // ...
    fcntl(server2_fd, F_SETFL, O_NONBLOCK);
    // ...

    // Create a vector of pollfd structs
    std::vector<pollfd> fds = {
        { server1_fd, POLLIN | POLLPRI | POLLERR | POLLHUP },
        { server2_fd, POLLIN | POLLPRI | POLLERR | POLLHUP }
    };

    while (true) {
        // Call poll on all the file descriptors
        int ret = poll(fds.data(), fds.size(), -1);
        if (ret < 0) {
            // Handle error
            perror("poll");
            break;
        }

        // Check which file descriptors have events
        for (auto& fd : fds) {
            if (fd.revents != 0) {
                // Handle the event
                if (fd.fd == server1_fd) {
                    // Handle connection on server 1
                } else if (fd.fd == server2_fd) {
                    // Handle connection on server 2
                }
            }
        }
    }

    // Close server sockets
    close(server1_fd);
    // ...
    close(server2_fd);
    // ...

    return 0;
}