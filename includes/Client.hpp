#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <sys/types.h>
#include <string>
#include <vector>
#include <ctime>
#include <map>
#include <time.h>

#include "Request.hpp"
#include "Cgi.hpp"

#define PACKAGE_SIZE 65563



class Client
{
    public:
        Client();
        void    ResetClient();
        
        
        int fd;                      // Client file descriptor
        bool initialResponseSent;    // Flag indicating if initial response headers have been sent
        int file_fd;                 // File descriptor for the requested file
        off_t content_length;        // Content length of the requested file
        int                             socket;
        bool                            server_full;
        void                     set_cgi_filename(Cgi &cgi);

        std::map<std::string, std::string>  request_header;
        size_t                              request_size;
        char                                request_chunk[PACKAGE_SIZE + 1];
        std::string                         full_request;
        Request                             response;
        bool                                cutoffClient;
        // struct sockaddr_in                  ip_address;
    private:
        std::string _cgi_filename;
};

#endif