#include "../includes/Client.hpp"

Client::Client() : fd(-1), initialResponseSent(false), file_fd(-1), content_length(0), socket(-1), server_full(false)
{}

Client& Client::operator=(const Client& other)
{
    if (this == &other) {
        return *this;
    }

    // Perform member-wise assignment
    this->fd = other.fd;
    this->initialResponseSent = other.initialResponseSent;
    this->file_fd = other.file_fd;
    this->content_length = other.content_length;
    this->socket = other.socket;
    this->server_full = other.server_full;
    this->request = other.request;
    this->response = other.response;

    return *this;
}

void    Client::ResetClient()
{
	fd = -1;
	initialResponseSent = false;
	file_fd = -1;
	content_length = 0;
	socket = -1;
	server_full = false;
	request.clear();
}

void	Client::CreateResponse(std::map<std::string, std::string> req, ServerConfig	conf)
{
	RequestProcessor tmp(req, conf);
	response = tmp;
}
