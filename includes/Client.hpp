#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <sys/types.h>
#include <string>
#include "RequestProcessor.hpp"
#include <map>

class Client {
    public:
        Client();
        
        Client& operator=(const Client& other);
        void    ResetClient();
        void	CreateResponse(std::map<std::string, std::string> req, ServerConfig	conf);
        int fd;                      // Client file descriptor
        bool initialResponseSent;    // Flag indicating if initial response headers have been sent
        int file_fd;                 // File descriptor for the requested file
        off_t content_length;        // Content length of the requested file
        int                             socket;
        bool                            server_full;
        std::map<std::string, std::string> request;
        RequestProcessor                   response;
};

#endif