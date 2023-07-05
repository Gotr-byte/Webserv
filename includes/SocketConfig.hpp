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
	
		static void	accessSocketBlock(std::fstream &config, int socket_no);
		static void accessServerBlock(std::fstream &config, int server_no);
		static void	removeWhitespaces(std::string & string);

		class ServerConfig
		{
			public:
				ServerConfig(std::string path, int socket_no, int server_no);
				ServerConfig();
				~ServerConfig();

				std::string									getConfProps(std::string key);
				std::string									getLocation(std::string location, std::string key);
				std::map<std::string, \
					std::map<std::string, std::string> > 	getLocations();

				std::map<std::string, \
					std::map<std::string, std::string> >	locations;

			private:
				void	setConfProps(std::string path, int socket_no, int server_no);
				void	setDefaultProps();

				bool	setLocations(std::string path, int socket_no, int server_no);
				void	checkLocationBlock(std::map<std::string, \
										std::string> & block, std::string dir);
				void	setDefaultLocation();

				std::map<std::string, std::string>			properties;
		};

		std::map<std::string, ServerConfig>	servers;

	private:

		void	setupPortHostServerNo(std::string path, int socket_no);
		void	setServerConfigs(std::string path, int socket_no, int server_no);

		int	servers_on_port;
};


