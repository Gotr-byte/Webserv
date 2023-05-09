#ifndef CONFIGCLASS_H
#define CONFIGCLASS_H

# include <map>
# include <vector>
# include <string>
# include <fcntl.h>
# include <utility>
# include <fstream>
# include <iostream>

class configClass
{
	public:
		configClass(std::string path, int socket_no);
		~configClass();

		std::string	getConfProps(std::string key);
		std::string	getLocation(std::string location, std::string);

	private:
		void	setConfProps(std::string path, int socket_no);
		bool	setLocations();
		void	setDefaultProps();
		void	setDefaultLocation();

		std::string									server_name;
		std::map<std::string, std::string>			properties;
		std::map<std::string, \
			std::map<std::string, std::string> >	locations;
};

#endif
