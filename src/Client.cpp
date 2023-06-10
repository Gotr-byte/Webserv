#include "../includes/Client.hpp"

Client::Client():
fd(-1), initialResponseSent(false), file_fd(-1),
content_length(0), socket(-1), server_full(false),
lastInteractionTime(time_t(-1)), cutoffClient(false){
}

void Client::ResetClient(){
	fd = -1;
	initialResponseSent = false;
	file_fd = -1;
	content_length = 0;
	socket = -1;
	server_full = false;
	lastInteractionTime = time_t(-1);
	cutoffClient = false;
	RequestVector.clear();
}

void Client::set_cgi_filename(Cgi &cgi){
	_cgi_filename = cgi.get_file_name();
}