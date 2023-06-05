#include "../includes/Client.hpp"

Client::Client() : fd(-1), initialResponseSent(false), file_fd(-1), content_length(0)
{}

void	Client::CreateResponse(std::map<std::string, std::string> req, ServerConfig	conf)
{
	RequestProcessor tmp(req, conf);
	response = tmp;
}
