#pragma once

# include "ServerConfig.hpp"
# include <string>
# include <map>
# include <sstream>
# include <dirent.h>
# include <cstdio>
# include <unistd.h>

# define BUF_SIZE 1024

class Request
{
	public:

		Request();

		void	CreateResponse(ServerConfig	conf);
		void	GenerateServerErrorResponse(int errorcode, ServerConfig	conf);
		void	GenerateUploadResponse();
		void	GenerateDeleteResponse();
		void	GenerateClientErrorResponse(std::string status, std::string issue);
		// void	generate_cgi_response()


		void	AssignLocation();
		void	setDate();
		void	PrepareGetResponse();
		void	PreparePost();
		void	PrepareDelete();
		void	setPostType();
		bool	CheckMethod();
		bool	CheckPath();
		bool	CheckExistance();
		bool	IsFile();
		bool	IsDirectory();
		void	SetResponseContentType();
		void	BuildResponseHeader();
		void	SetupErrorPage(std::string status, std::string issue);
		void	ObtainFileLength();
		void	CreateAutoindex();
		bool	CheckPermissions();
		bool	CheckBodySize();
		void	generate_cgi_response(std::string path_to_HTML);
		// void	ObtainCgiFileLength()

		static int		nextId;
		int		id;
		bool	initialResponseSent;
		int		file_fd;
		int 	client_fd;
		bool	requestdone;
		bool	error;
		std::string	method;
		off_t		contentlength;
		std::string	ResponseHeader;
		std::string	responsebody;
		std::string	path;
		std::string	contenttype;
		std::string	statuscode;
		bool		cutoffClient;
		bool		isCGI;
		bool		isUpload;
		bool		isDelete;
		std::map<std::string, std::string>	requestHeader;
		ServerConfig						config;

	private:

		std::string							clientpath;
		bool								isdirectory;
		bool								autoindex;
		std::string	date;
		std::string	protocoll;
		std::string	port;
		std::string additionalinfo;
};