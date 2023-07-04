#pragma once

# include "ServerConfig.hpp"
# include <string>
# include <map>
# include <sstream>
# include <dirent.h>
# include <cstdio>
# include <unistd.h>

#define BUF_SIZE 1024

class Response
{
	public:

		Response();

		void	generateUploadResponse(std::string file_path);
		void	generateDeleteResponse();
		void	generateErrorResponse(std::string status, std::string issue);
		void	generateRedirectionResponse(std::string URL);
		// void	generate_cgi_response()

		void	setDate();
		void	setResponseContentType(std::string path);
		void	buildResponseHeader();
		void	setupErrorPage(std::string status, std::string issue);
		void	obtainFileLength(std::string path);
		void	createAutoindex(std::string path);
		void	generateCgiResponse(std::string path);
		// void	ObtainCgiFileLength()

		bool		is_chunked;
		std::string	header;
		std::string	body;
		std::string	content_type;
		std::string	status_code;
		std::string	error_path;
		std::string	server_name;
		off_t		content_length;

	private:

		std::string	date;
		std::string	protocoll;
		std::string additional_info;
};