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
		std::string	getLocation(std::string location, std::string key);

	private:
		void	setConfProps(std::string path, int socket_no);
		bool	setLocations(std::string path, int socket_no);
		void	checkLocationBlock(std::map<std::string, \
								std::string> & block, std::string dir);
		void	setDefaultProps();
		void	setDefaultLocation();
		void	accessServerBlock(std::fstream	& config, int socket_no);
		void	removeWhitespaces(std::string	& string);

		std::string									server_name;
		std::map<std::string, std::string>			properties;
		std::map<std::string, \
			std::map<std::string, std::string> >	locations;
};

#endif
