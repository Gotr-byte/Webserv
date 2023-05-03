#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

class Configuration
{
    public:
    int parseSetListen(std::string filename, std::string parameter);

  unsigned int    listen; // port 80;

};
//There will be more confifuration variabkles to add, this object parses the configuration file ang gets parameter values from the file.
//It currently sets the listening port in accordance with the config file.
// private:
// bool server_tokens; //off
// std::string include; // /etc/nginx/mime.types;

// std::string     name //example.com;
// std::string     root // /var/www/example.com;
// std::string     index //index.html;
// std::string     server_location;// / {
// try_files $uri $uri/ /index.html;
// }
    
// std::string    api_location;  ///api/ {
// std::string    proxy_pass; // http://loclalhost:3000/;