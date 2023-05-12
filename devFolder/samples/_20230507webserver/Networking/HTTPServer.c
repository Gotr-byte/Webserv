#include "HTTPServer.h"
#include "HTTPRequest.h"

void register_routes(struct HTTPServer *server, void (*route_function)(struct HTTPServer *server, struct HTTPRequest *request), char *uri, int num_methods, ...);

void launch(struct Server *);

struct HTTPServer http_server_constructor()
{
    struct HTTPServer server;

    server.server = server_constructor(AF_INET, SOCK_STREAM, 0 , INADDR_ANY, 80, 255, );
    server.routes = dictionary_constructor(compare_string_keys);
    server.register_routes = register_routes;
    return server;
}

void register_routes(struct HTTPServer *server, void (*route_function)(struct HTTPServer *server, struct HTTPRequest *request), char *uri, int num_methods, ...)
{
    
}