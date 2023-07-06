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

		void	generateCgiResponse();
		void	generateDeleteResponse();
		void	generateErrorResponse(std::string status, std::string issue);
		void	generateRedirectionResponse(std::string URL);
		void	generateUploadResponse(std::string file_path);
		void	buildResponseHeader();
		void	setResponseContentType(std::string path);
		void	createAutoindex(std::string path);

		std::string	body;
		std::string	header;
		std::string	server_name;
		off_t		content_length;
		bool		is_chunked;

	private:
		void	setDate();

		std::string additional_info;
		std::string	content_type;
		std::string	date;
		std::string	protocoll;
		std::string	status_code;
};