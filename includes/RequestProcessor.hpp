#ifndef REQUESTPROCESSOR_H
#define REQUESTPROCESSOR_H

# include <string>
# include "../includes/ServerConfig.hpp"
// # include 

class RequestProcessor
{
	public:
		RequestProcessor();
		// RequestProcessor(std::map<std::string, std::string> request, );
		~RequestProcessor();

	private:
		RequestProcessor();

		std::string	port;
		std::string	method;
		std::string	filepath;
		std::string	contenttype;
		std::string	statuscode;
		std::string	content_length;

};
#endif
