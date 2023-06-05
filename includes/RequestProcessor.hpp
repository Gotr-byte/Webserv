#ifndef REQUESTPROCESSOR_H
#define REQUESTPROCESSOR_H

# include <string>
# include <map>
# include <dirent.h>
# include <cstdio>
# include <unistd.h>
# include "../includes/ServerConfig.hpp"
// # include 

class RequestProcessor
{
	public:
		RequestProcessor(std::map<std::string, std::string> req, ServerConfig	conf);
		~RequestProcessor();
		void	AssignLocation();
		void	setDate();
		bool	CheckMethod();
		bool	CheckPath();
		bool	CheckExistance();
		bool	IsFile();
		bool	IsDirectory();
		void	SetupErrorPage(std::string status, std::string issue);
		void	ObtainFileLength();

	private:

		std::map<std::string, std::string>	request;
		ServerConfig						config;
		std::string							clientlocation;
		bool								isdirectory;
		bool								autoindex;
		std::string	date;
		std::string	protocoll;
		std::string	port;
		std::string	method;
		std::string	path;
		std::string	contenttype;
		std::string	statuscode;
		off_t		contentlength;
		std::string	ResponseHeader;
		std::string additionalinfo;
};
#endif
