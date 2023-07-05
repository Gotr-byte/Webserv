#pragma once

# include <map>
# include <vector>
# include <string>
# include <fcntl.h>
# include <utility>
# include <fstream>
# include <iostream>
# include <stdlib.h>
class ServerConfig;

class SocketConfig
{
	public:
		SocketConfig(std::string path, int socket_no);
		~SocketConfig();
	
		std::string	port;
		std::string	host;

		std::map<std::string, ServerConfig>	servers;

	private:

		void	setServerConfigs(std::string path, int socket_no);
		static void	accessSocketBlock(std::fstream &config, int socket_no);
		static void	removeWhitespaces(std::string	& string);

	public:
		class ServerConfig
		{
			public:
				ServerConfig(std::string path, int socket_no);
				~ServerConfig();

				std::string	getConfProps(std::string key);
				std::string	getLocation(std::string location, std::string key);
				std::map<std::string, \
					std::map<std::string, std::string> > getLocations();
				std::map<std::string, \
					std::map<std::string, std::string> >	locations;

				std::string	port;
				std::string	host;

			private:
				void	setConfProps(std::string path, int socket_no);
				bool	setLocations(std::string path, int socket_no);
				void	checkLocationBlock(std::map<std::string, \
										std::string> & block, std::string dir);
				void	setDefaultProps();
				void	setDefaultLocation();
				void	accessServerBlock(std::fstream	& config, int socket_no);

				std::map<std::string, std::string>			properties;
		};
};


