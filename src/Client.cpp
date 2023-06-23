#include "../includes/Client.hpp"

Client::Client():
fd(-1), initialResponseSent(false), file_fd(-1),
content_length(0), socket(-1), server_full(false), cutoffClient(false){
}

void Client::ResetClient(){
	fd = -1;
	initialResponseSent = false;
	file_fd = -1;
	content_length = 0;
	socket = -1;
	server_full = false;
	cutoffClient = false;
	full_request.clear();
	memset(request_chunk, 0, PACKAGE_SIZE);
}

void Client::set_cgi_filename(Cgi &cgi){
	_cgi_filename = cgi.get_file_name();
}