#include "../includes/Client.hpp"

Client::Client(int server_idx, ServerConfig conf) : server_index(server_idx), config(conf), initialResponseSent(false), file_fd(-1),
content_length(0), cutoffClient(false), color_index(1) {}

void    Client::create_response()
{
	response.CreateResponse(config);
}

void    Client::check_request()
{
	
}

void    Client::mapping_request_header()
{
    int new_line_count = 0;
    // Get lines of Header
    int n;

	std::size_t headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
		perror("no correct header format");
        // Handle error: invalid HTTP request without a 

	std::string header = request.substr(0, headerEnd);
	request = request.substr(headerEnd + 4);

	request_size -= headerEnd + 4;

    std::size_t lineStart = 0;
    std::size_t lineEnd;

    lineEnd = header.find("\r\n", lineStart);
	std::string line = header.substr(lineStart, lineEnd - lineStart);
	lineStart = lineEnd + 2;

	 // Extract a line from the header
    if (!line.empty())
    {
        request_header["method:"] = std::strtok(&line[0], " ");
        request_header["location:"] = std::strtok(NULL, " ");
        std::size_t found = request_header["location:"].find('?');
        if (found != std::string::npos && request_header["method:"] == "GET")
		{
            request_header["HTTP_version:"] = std::strtok(NULL, " ");
            std::string temporary = std::strtok(&request_header["location:"][0], "?");
            request_header["query_string:"] = std::strtok(NULL, " ");
            request_header["location:"] = temporary;
        }
        else
            request_header["HTTP_version:"] = std::strtok(NULL, " ");
    }
	while ((lineEnd = header.find("\r\n", lineStart)) != std::string::npos)
    {
		std::string line = header.substr(lineStart, lineEnd - lineStart);
        tokenizing(request_header, line);
		lineStart = lineEnd + 2;
    }
    size_t v;
    if ((v = request_header["Content-Type:"].find("boundary=")) != std::string::npos)
    {
        std::string key = request_header["Content-Type:"].substr(v, 8);
        std::string value = request_header["Content-Type:"].substr(request_header["Content-Type:"].find("=") + 1);
        request_header[key] = value;
        request_header["Content-Type:"] = request_header["Content-Type:"].substr(0, request_header["Content-Type:"].find(";"));
    }
    print_request(request_header);
}

void Client::print_request(std::map<std::string, std::string> my_map)
{
    std::map<std::string, std::string>::iterator it;
    switch (color_index) {
        case 0:
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                std::cout << RED << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
            }
            color_index++;
            break;
        case 1:
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                std::cout << YELLOW << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
            }
            color_index++;
            break;
        case 2:
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                std::cout << CYAN << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
            }
            color_index++;
            break;
        case 3:
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                std::cout << CYAN << "Key: " << it->first << ", Value: " << it->second << DEF <<std::endl;
            }
            color_index = 0;
            break;
        default:
            std::cout << "Invalid choice." << std::endl;
            break;
    }
}

/**
 * Tokenizes a line of text and stores the key-value pair in a map.
 *
 * @param request The map to store the key-value pair.
 * @param line_to_tokenize The line of text to tokenize.
 */
void Client::tokenizing(std::map<std::string, std::string> &request, std::string line_to_tokenize)
{
    std::stringstream tokenize_stream(line_to_tokenize);
    std::string value;
    std::string key;
    key = line_to_tokenize.substr(0, line_to_tokenize.find(":") + 1);
    value = line_to_tokenize.substr(line_to_tokenize.find(":") + 1);
    removeWhitespaces(key);
    removeWhitespaces(value);
    request[key] = value;
}

void Client::removeWhitespaces(std::string &string)
{
    string.erase(0, string.find_first_not_of(" \t"));
    string.erase(string.find_last_not_of(" \t") + 1);
}



void	Client::set_request(char *chunk, size_t buffer_length)
{
	request_size += buffer_length;
    for (size_t size = 0; size < buffer_length; size++)
        request.push_back(chunk[size]);
	if (request_size < PACKAGE_SIZE)
		request_complete = true;
	std::cout << request << std::endl;
}


void Client::set_cgi_filename(Cgi &cgi){
	_cgi_filename = cgi.get_file_name();
}