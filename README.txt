A project recreating the works of HTTP Webserver.

//TODO make a page
//TODO perform the operations required by the documentation on NGINX, and note down the behaviour, also compare later
//TODO needs request parsing
//TODO needs response generating
//TODO error response
//TODO response
//TODO needs HTML, just a page with a file to download
//TODO check if the sockets need to be non blocking
//TODO send a simple error code response
//TODO check if the server has a asynchronous I/O
//TODO handle routing in the webserver
//TODO URL header to a hash table?

// server {
//     port: 1896;
//     host: localhost;
//     server_name: test_server;
//     error_page: ./www/errors/;
//     limit_body_size: 100;
//     allowed_methods: GET;
//     location / {
//         root: ./www/html/;
//         index: index.html;
//         redirect: https://google.de/;
//         allowed_methods: POST, GET;
//         autoindex: on;
//     }
// }

// Yes, in order for the operations on the server to be non-blocking, the sockets need to be set up in a specific way.

// First, the server socket should be set to non-blocking mode by using the fcntl() function to set the O_NONBLOCK flag on the socket file descriptor. This allows the server to accept incoming connections without blocking, even if there are no pending connections.

// Next, when a new client connection is accepted, the socket for that connection should also be set to non-blocking mode using the same method. This allows the server to read and write to the socket without blocking, even if the client is not ready to send or receive data.

// Finally, the server should use a non-blocking I/O model, such as the epoll() or select() system calls, to handle multiple client connections without blocking. These system calls allow the server to monitor multiple sockets for I/O activity and respond to them asynchronously, without waiting for any particular socket to complete its I/O operation.

// In summary, setting the sockets to non-blocking mode and using a non-blocking I/O model are the key steps to making the server operations non-blocking. This allows the server to handle multiple connections simultaneously and efficiently, without blocking on any particular connection.

//Handle various pages
// New client connected
// Received message from client: GET /favicon.ico HTTP/1.1
// Host: localhost:8088
// Connection: keep-alive
// sec-ch-ua: "Google Chrome";v="111", "Not(A:Brand";v="8", "Chromium";v="111"
// sec-ch-ua-mobile: ?0
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/111.0.0.0 Safari/537.36
// sec-ch-ua-platform: "macOS"
// Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8
// Sec-Fetch-Site: same-origin
// Sec-Fetch-Mode: no-cors
// Sec-Fetch-Dest: image
// Referer: http://localhost:8088/index.html
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en-US,en;q=0.9



// Sure thing, fellow adventurer! When we set a socket to non-blocking, it means that when we call functions like recv or send, they will return immediately, even if there is no data available to receive or the socket buffer is full and cannot send any more data. This is different from the default blocking mode, where these functions will wait until there is data available to receive or the buffer has space to send data.

// To set a socket to non-blocking mode, we can use the fcntl function with the F_SETFL command and the O_NONBLOCK flag. Here's an example code snippet:

// c++
// Copy code
// int flags = fcntl(sock_fd, F_GETFL, 0);
// flags |= O_NONBLOCK;
// fcntl(sock_fd, F_SETFL, flags);
// In this example, sock_fd is the socket file descriptor that we want to set to non-blocking mode. We first use fcntl with the F_GETFL command to get the current socket flags. We then bitwise OR the O_NONBLOCK flag with the current flags to set the non-blocking mode, and finally use fcntl with the F_SETFL command to set the new flags.

// By setting the socket to non-blocking mode, we can then use the poll function to wait for events on the socket without blocking the main thread of our program. This allows us to handle multiple connections at the same time, which is useful for server applications.

// I hope that helps! Let me know if you have any more questions.

// int create_socket(int etc_port,  struct sockaddr_in *server_addr)
// {
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd < 0) {
//         perror("Error creating server socket");
//         exit(EXIT_FAILURE);
//     }

//     //set the socket to non blocking
//     fcntl(server_fd, F_SETFL, O_NONBLOCK);

//     // Set the server socket options
//     int opt = 1;
//     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
//         perror("Error setting server socket options");
//         exit(EXIT_FAILURE);
//     }

//     // Bind the server socket to a specific port
//     // struct sockaddr_in server_addr;
//     // memset(&server_addr, 0, sizeof(server_addr));
//     // server_addr.sin_family = AF_INET;
//     // server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr->sin_port = htons(etc_port);
//     if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(*server_addr)) < 0) {
//         perror("Error binding server socket etc");
//         exit(EXIT_FAILURE);
//     }

//     // Listen for incoming connections on the server socket
//     if (listen(server_fd, MAX_CLIENTS) < 0) {
//         perror("Error listening on server socket");
//         exit(EXIT_FAILURE);
//     }

//     return(server_fd);
// }