#ifndef REQUESTPROCESSOR_H
#define REQUESTPROCESSOR_H

# include <string>
# include <map>
# include <sstream>
# include <dirent.h>
# include <cstdio>
# include <unistd.h>
# include "../includes/ServerConfig.hpp"
// # include 

class RequestProcessor
{
	public:
		RequestProcessor();
		RequestProcessor(std::map<std::string, std::string> req, ServerConfig	conf);
		~RequestProcessor();
		void	AssignLocation();
		void	setDate();
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
		std::string	method;
		off_t		contentlength;
		std::string	ResponseHeader;
		std::string	autoindexbody;
		std::string	path;
		std::string	contenttype;

	private:

		std::map<std::string, std::string>	request;
		ServerConfig						config;
		std::string							clientpath;
		bool								isdirectory;
		bool								autoindex;
		std::string	date;
		std::string	protocoll;
		std::string	port;
		std::string	statuscode;
		std::string additionalinfo;
};
#endif
