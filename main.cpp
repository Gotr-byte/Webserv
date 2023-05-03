#include <iostream>
#include <fstream>
#include "./includes/Configuration.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

#define MAX_CLIENTS 5

int handleRequest(int port) {
      // Create socket
      int server_fd = socket(AF_INET, SOCK_STREAM, 0);
      if (server_fd < 0) {
          std::cerr << "Failed to create socket\n";
          exit(EXIT_FAILURE);
      }

      // Bind socket to address
      struct sockaddr_in address;
      memset(&address, 0, sizeof(address));
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = INADDR_ANY;
      address.sin_port = htons(port);
      if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
          std::cerr << "Failed to bind socket to address\n";
          exit(EXIT_FAILURE);
      }

      // Listen for incoming connections
      if (listen(server_fd, MAX_CLIENTS) < 0) {
          std::cerr << "Failed to listen for incoming connections\n";
          exit(EXIT_FAILURE);
      }

      // Create array of pollfd structs for monitoring sockets
      struct pollfd poll_fds[MAX_CLIENTS + 1];
      memset(poll_fds, 0, sizeof(poll_fds));
      poll_fds[0].fd = server_fd;
      poll_fds[0].events = POLLIN;

      // Loop and wait for incoming connections or data
      int num_fds = 1;
      while (true) {
          // Use poll to monitor sockets for incoming data or connections
          if (poll(poll_fds, num_fds, -1) < 0) {
              std::cerr << "Failed to poll sockets\n";
              exit(EXIT_FAILURE);
          }

          // Check for incoming connections on the server socket
          if (poll_fds[0].revents & POLLIN) {
              int client_fd = accept(server_fd, NULL, NULL);
              if (client_fd < 0) {
                  std::cerr << "Failed to accept incoming connection\n";
                  exit(EXIT_FAILURE);
              }

              // Add client socket to pollfd array
              poll_fds[num_fds].fd = client_fd;
              poll_fds[num_fds].events = POLLIN;
              num_fds++;

              std::cout << "New client connected\n";
          }

          // Check for incoming data on client sockets
          for (int i = 1; i < num_fds; i++) {
              if (poll_fds[i].revents & POLLIN) {
                  char buffer[1024] = {0};
                  if (recv(poll_fds[i].fd, buffer, sizeof(buffer), 0) <= 0) {
                      // Client disconnected
                      close(poll_fds[i].fd);
                      poll_fds[i].fd = -1;
                  } else {
                      // Print received data
                      std::cout << "Received data: " << buffer << std::endl;
                  }
              }
          }

          // Remove disconnected clients from pollfd array
          int i = 1;
          while (i < num_fds) {
              if (poll_fds[i].fd == -1) {
                  for (int j = i; j < num_fds - 1; j++) {
                      poll_fds[j].fd = poll_fds[j + 1].fd;
                  }
                  num_fds--;
              } else {
                  i++;
              }
          }
      }
      return 0;
  }

int main (int argc, char **argv)
{
  if (argc != 2)
  {
      std::cerr << "Error: invalid number of arguments, please enter one argument\n";
      return 1;
  }
  Configuration conf;
  if (conf.parseSetListen(argv[1], "listen"))
    return 2;
  std::cout << conf.listen << "\n";
  handleRequest(conf.listen);
}