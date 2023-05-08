#ifndef OPEN_PORT_SOCKET_HPP
# define OPEN_PORT_SOCKET_HPP

# include <iostream>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <signal.h>
# include <iostream>
# include <string>
# include <map>
# include <sstream>
# include <list>
# include <vector>
# include <poll.h>
# include <fcntl.h>
# include <cerrno>
# include <sys/types.h>
# include <algorithm>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/time.h>
# include <fstream>
# include <limits>
# include <dirent.h>
# include <signal.h>

# define SUCESS 0
# define FAILED_SOCKET -1
# define LIST_END NULL

# define MAX_REQUEST_QUE 10

// class	BindFailure: public std::exception {};

int	openPortSocket(char const *port_number);

#endif