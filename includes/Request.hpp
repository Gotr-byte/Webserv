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
		void	GenerateOverloadError(int errorcode, ServerConfig	conf);

		void	AssignLocation();
		void	setDate();
		void	setPostType();
		bool	CheckMethod();
		bool	CheckPath();
		bool	CheckExistance();
		bool	IsFile();
		bool	IsDirectory();
		void	SetContentType();
		void	BuildResponseHeader();
		void	SetupErrorPage(std::string status, std::string issue);
		void	ObtainFileLength();
		void	CreateAutoindex();
		bool	CheckPermissions();
		bool	CheckBodySize();

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
		std::string	autoindexbody;
		std::string	path;
		std::string	contenttype;
		std::string	statuscode;
		bool		cutoffClient;
		bool		isCGI;
		bool		isUpload;
		std::map<std::string, std::string>	requestHeaderMap;

	private:

		ServerConfig						config;
		std::string							clientpath;
		bool								isdirectory;
		bool								autoindex;
		std::string	date;
		std::string	protocoll;
		std::string	port;
		std::string additionalinfo;
};