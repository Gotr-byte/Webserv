#include <unistd.h>
#include <fcntl.h>

int main()
{
    char buffer[256];
    int num_bytes = 256;
    int fd = open ("example.txt", O_RDONLY | O_NONBLOCK);
    int bytes_read = read(fd, buffer, num_bytes);
    close(fd);
}