#include "../includes/Client.hpp"

Client::Client() : fd(-1), initialResponseSent(false), file_fd(-1), content_length(0)
{}

