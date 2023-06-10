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




class Client
{
    public:
        Client();
        void    ResetClient();
        void	CreateResponse(std::map<std::string, std::string> req, ServerConfig	conf);
        
        
        int fd;                      // Client file descriptor
        bool initialResponseSent;    // Flag indicating if initial response headers have been sent
        int file_fd;                 // File descriptor for the requested file
        off_t content_length;        // Content length of the requested file
        int                             socket;
        bool                            server_full;
        time_t                          lastInteractionTime;
        void                     set_cgi_filename(Cgi &cgi);

        std::vector<Request>                RequestVector;
        bool                                cutoffClient;
    private:
        std::string _cgi_filename;
};

#endif