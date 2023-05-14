#ifndef Route_h
#define Route_h

#include "HTTPServer.h"
#include "HTTPRequest.h"

struct Route
{
    int methods[9];
    char* uri;
    char* (*route_function)(struct HTTPServer *server, struct HTTPRequest request);
};

struct HTTPServer http_server_constructor();    
#endif