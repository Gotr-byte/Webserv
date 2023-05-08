#include "HTTPRequest.h"
#include <string.h>
#include <stdlib.h>

int method_select(char *method)
{
    if (strcmp(method, "GET") == 0)
    {
        return GET;
    }
    else if(strcmp(method, "POST") == 0)
    {
        return POST;
    }
    else if(strcmp(method, "PUT") == 0)
    {
        return PUT;
    }
    else if(strcmp(method, "HEAD") == 0)
    {
        return HEAD;
    }
    else if(strcmp(method, "CATCH") == 0)
    {
        return CATCH;
    }
    else if(strcmp(method, "DELETE") == 0)
    {
        return DELETE;
    }
    else if(strcmp(method, "CONNECT") == 0)
    {
        return CONNECT;
    }
    else if(strcmp(method, "OPTIONS") == 0)
    {
        return OPTIONS;
    }
    else if(strcmp(method, "TRACE") == 0)
    {
        return TRACE;
    }
    else
    {
        return -1;
    }
}

struct HTTPRequest http_request_constructor(char* request_string)
{
    struct HTTPRequest request;
    char requested[strlen(request_string)];
    strcpy(requested, request_string);
    for(int i = 0; i < srtlen(requested) - 2; i++)
    {
        if (requested[i] == '\n' && requested[i + 1] == '\n')
        {
            requested[i + 1] = '|';
        }
    }
    char *request_line = strtok(requested, "\n");
    char *header_fields = strtok(NULL, "|");
    char *body = strtok(NULL, "|");


    char * method = strtok(request_line, "");
    request.Method = method_select(method);
    char *URI = strtok(NULL, "");
    request.URI = URI;
    char *HTTPVersion = strtok(NULL, "/");
    HTTPVersion = strtok(HTTPVersion, "/");
    HTTPVersion = strtok(NULL, "/");
    request.HTTPVersion = (float)atof(HTTPVersion);
    return request;
}