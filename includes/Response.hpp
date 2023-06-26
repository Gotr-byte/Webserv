#pragma once

# include "ServerConfig.hpp"
# include <string>
# include <map>
# include <sstream>
# include <dirent.h>
# include <cstdio>
# include <unistd.h>

class Response
{
	public:

		Response();

		void	CreateResponse(ServerConfig	conf);
		void	GenerateServerErrorResponse(int errorcode, ServerConfig	conf);
		void	GenerateUploadResponse();
		void	GenerateDeleteResponse();
		void	GenerateClientErrorResponse(std::string status, std::string issue);
		// void	generate_cgi_response()

		void	setDate();
		void	SetResponseContentType(std::string path);
		void	BuildResponseHeader();
		void	SetupErrorPage(std::string status, std::string issue);
		void	ObtainFileLength(std::string path);
		void	CreateAutoindex(std::string path);
		void	generate_cgi_response(std::string path_to_HTML);
		// void	ObtainCgiFileLength()

		static int		nextId;
		int		id;
		off_t		contentlength;
		std::string	header;
		std::string	body;
		std::string	contenttype;
		std::string	statuscode;
		std::string	error_path;
		std::string	server_name;

	private:

		std::string	clientpath;
		std::string	date;
		std::string	protocoll;
		std::string	port;
		std::string additionalinfo;
};