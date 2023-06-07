#include "../includes/Client.hpp"

Client::Client() : fd(-1), initialResponseSent(false), file_fd(-1), content_length(0), socket(-1), server_full(false), lastInteractionTime(std::time_t(-1))
{}

void    Client::ResetClient()
{
	fd = -1;
	initialResponseSent = false;
	file_fd = -1;
	content_length = 0;
	socket = -1;
	server_full = false;
	lastInteractionTime = std::time_t (-1);
	request.clear();
}

void    Client::SetupForNewInteraction()
{
	initialResponseSent = false;
	content_length = 0;
	file_fd = -1;
	server_full = false;
	request.clear();
}

void	Client::CreateResponse(std::map<std::string, std::string> req, ServerConfig	conf)
{
	RequestProcessor tmp(req, conf);
	response = tmp;
}
