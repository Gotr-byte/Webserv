#pragma once

#include <sys/types.h>
#include <string>
#include <vector>
#include <ctime>
#include <map>
#include <time.h>
#include <string.h>
#include <cstring>
#include <stdlib.h>
#include "Response.hpp"
#include "SocketConfig.hpp"

#define PACKAGE_SIZE 5000

class Client
{
	public:
		Client(SocketConfig conf, std::string ip);
		~Client();

		void	setRequest(char *chunk, size_t buffer_length);
		bool	mapRequestHeader();
		void	tokenizeRequestHeader(std::map<std::string, std::string> & request, std::string line_to_tokenize);
		void	removeWhitespaces(std::string &string);
		void	printRequest(std::map<std::string, std::string> my_map);
		void	checkServerConfig(ServerConfig config);
		void	checkRequest();
		void	assignLocation();
		void	assignServer();
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
		bool	obtainFileLength();



		SocketConfig						config;
		Response                            response;
		std::map<std::string, std::string>  request_header;

		int                             	client_fd;
		int									color_index;
		int                             	file_fd;
		int									id;
		int                             	server_index;

		std::string                         cgi_path;
		std::string                         client_ip;
		std::string                         location;
		std::string                         method;
		std::string                         path_info;
		std::string                         path_on_client;
		std::string                         path_on_server;
		std::string                         redirect_url;
		std::string                         request;
		std::string                         server_name;


		bool                                autoindex;
		bool                                cancel_recv;
		bool 								header_sent;
		bool                        		is_cgi;
		bool                        		is_delete;
		bool                                is_get;
		bool                                is_redirect;
		bool                        		is_upload;
		bool                                last_chunk_sent;
		bool                                query_string;
		bool                                request_complete;
		bool                                request_processed;
		bool                                response_sent;
		
	private:
		SocketConfig::ServerConfig			server_config;

		static int  						nextId;
		std::string                         cgi_extension;
		size_t                              request_size;
};
