#pragma once

class Client {
    public:
        Client():fd(-1), initialResponseSent(false), file_fd(-1), content_length(0){};
        int fd;                      // Client file descriptor
        bool initialResponseSent;    // Flag indicating if initial response headers have been sent
        int file_fd;                 // File descriptor for the requested file
        off_t content_length;        // Content length of the requested file
};