#pragma once

class Client {
    public:
        Client():fd(-1), initialResponseSent(false), content_length(0), file_fd(-1){};
        int fd;                      // Client file descriptor
        bool initialResponseSent;    // Flag indicating if initial response headers have been sent
        int file_fd;                 // File descriptor for the requested file
        off_t content_length;        // Content length of the requested file
};