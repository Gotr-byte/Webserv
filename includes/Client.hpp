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
#include "ServerConfig.hpp"

#define PACKAGE_SIZE 65563



class Client
{
    public:
        Client(int server_idx, ServerConfig conf);
        void    set_request(char *chunk, size_t buffer_length);
        void    mapping_request_header();
        void    tokenizing(std::map<std::string, std::string> & request, std::string line_to_tokenize);
        void    removeWhitespaces(std::string &string);
        void    print_request(std::map<std::string, std::string> my_map);
        void    check_server_config(ServerConfig config);
        void    check_request();
        void    create_response();


        int color_index;

        bool initialResponseSent;    // Flag indicating if initial response headers have been sent
        int file_fd;                 // File descriptor for the requested file
        off_t content_length;        // Content length of the requested file
        int                             server_index;
        int                             client_fd;
        void                            set_cgi_filename(Cgi &cgi);

        ServerConfig                        config;
        bool                                request_complete;
        std::map<std::string, std::string>  request_header;
        size_t                              request_size;
        std::string                         request;
        Request                             response;
        bool                                cutoffClient;
        // struct sockaddr_in                  ip_address;
    private:
        std::string _cgi_filename;
};

#endif