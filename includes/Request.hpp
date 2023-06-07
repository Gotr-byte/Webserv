#pragma once

# include "RequestProcessor.hpp"
# include "ServerConfig.hpp"
# include <string>
# include <map>

class Request
{
	public:

	Request();

	void	CreateResponse(ServerConfig	conf);
	
	std::map<std::string, std::string> 	request;
	RequestProcessor 					response;
	bool operator<(const Request& other);

	static int		nextId;
	int		id;
	bool	initialResponseSent;
	int		file_fd;
	int 	client_fd;
	bool	requestdone;
};