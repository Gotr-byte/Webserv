#ifndef HTTPRequest_h
#define HTTPRequest_h

enum HTTPMethods{
    GET,
    POST,
    PUT,
    HEAD,
    CATCH,
    PATCH,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE
};

struct HTTPRequest
{
    int Method;
    char *URI;
    float HTTPVersion;

};

struct HTTPRequest http_request_constructor(char* request_string);

#endif