#include <iostream>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fileDescriptor = open("request_body_4", O_RDONLY);
    if (fileDescriptor == -1) {
        std::cerr << "Failed to open input file." << std::endl;
        return 1;
    }

    // Redirect the file descriptor to standard input (file descriptor 0)
    if (dup2(fileDescriptor, 0) == -1) {
        std::cerr << "Failed to redirect file descriptor." << std::endl;
        return 1;
    }

    // Close the original file descriptor
    close(fileDescriptor);

    // Read and print the content from standard input
    char c;
    while (std::cin.get(c)) {
        std::cout.put(c);
    }

    return 0;
}