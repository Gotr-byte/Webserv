#pragma once

#include <sys/types.h>
#include <string>
#include <vector>
#include <ctime>
#include <map>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "Response.hpp"
#include "ServerConfig.hpp"

#define PACKAGE_SIZE 5000

class Client
{
	public:
		Client(ServerConfig conf, std::string ip);
		void	setRequest(char *chunk, size_t buffer_length);
		bool	mapRequestHeader();
		void	tokenizeRequestHeader(std::map<std::string, std::string> & request, std::string line_to_tokenize);
		void	removeWhitespaces(std::string &string);
		void	printRequest(std::map<std::string, std::string> my_map);
		void	checkServerConfig(ServerConfig config);
		void	checkRequest();
		void	assignLocation();
		void	setError(std::string code);
		bool	checkMethod();
		void	resetProperties();
		bool	checkExistance();
		void	prepareGet();
		void	preparePost();
		void	prepareDelete();
		void	prepareRedirect();
		bool	isDirectory();
		void	parseClientPath();
		void	closeFileFd();
		bool	isHeaderValid();

		static int  nextId;
		int		id;
		int color_index;

		bool header_sent;    // Flag indicating if initial response headers have been sent
		off_t content_length;        // Content length of the requested file
		int                             server_index;
		int                             client_fd;
		int                             file_fd;

		ServerConfig                        config;
		bool                                request_complete;
		std::map<std::string, std::string>  request_header;
		size_t                              request_size;
		std::string                         request;
		Response                            response;
		bool                                is_error;
		std::string                         method;
		std::string                         path_on_server;
		std::string                         path_on_client;
		std::string                         path_info;
		std::string                         server_name;
		std::string                         location;
		std::string                         client_ip;
		std::string                         redirect_url;
		std::string                         cgi_extension;
		std::string                         cgi_path;
		bool                                kill_client;
		bool                                autoindex;
		bool                                cancel_recv;
		bool                                is_redirect;
		bool                        		is_cgi;
		bool                                is_get;
		bool                        		is_upload;
		bool                        		is_delete;
		bool                                query_string;
		bool                                response_sent;
		bool                                last_chunk_sent;
		bool                                request_processed;
		// struct sockaddr_in                  ip_address;
	private:
		std::string _cgi_filename;
};
