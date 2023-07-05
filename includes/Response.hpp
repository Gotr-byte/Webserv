#pragma once

# include <string>
# include <map>
# include <sstream>
# include <iostream>
# include <dirent.h>
# include <cstdio>
# include <unistd.h>

#define BUF_SIZE 1024

class Response
{
	public:

		Response();
		~Response();

		void	buildResponseHeader();
		void	generateUploadResponse(std::string file_path);
		void	generateDeleteResponse();
		void	generateErrorResponse(std::string status, std::string issue);
		void	generateRedirectionResponse(std::string URL);
		void	generateCgiResponse();

		void	createAutoindex(std::string path);
		void	setupErrorPage(std::string status, std::string issue);
		void	setDate();
		void	setResponseContentType(std::string path);

		std::string	body;
		std::string	header;
		std::string	server_name;
		off_t		content_length;
		bool		is_chunked;

	private:

		std::string additional_info;
		std::string	content_type;
		std::string	date;
		std::string	protocoll;
		std::string	status_code;
};