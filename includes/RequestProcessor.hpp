#ifndef REQUESTPROCESSOR_H
#define REQUESTPROCESSOR_H

# include <string>
// # include 

class RequestProcessor
{
	public:
		RequestProcessor(std::string const & code);
		~RequestProcessor();

	private:
		RequestProcessor();

		std::string	method;
		std::string	filepath;
		std::string	contenttype;
		std::string	statuscode;

};
#endif
