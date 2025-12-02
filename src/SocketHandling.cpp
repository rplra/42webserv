#include "Webserv.hpp"

int createAllListeningSockets(const std::vector<Server>& servers, 
    std::vector<int>& serverSockets, std::map<int, const Server*>& socketServerMap) 
{
    for (size_t i = 0; i < servers.size(); ++i) {
        int serverSocket = createListeningSocket(servers[i].getHost(), servers[i].getPort());
        if (serverSocket < 0) {
            std::cerr << RED << "Failed to create listening socket for server on port " << servers[i].getPort() << RESET << std::endl;
            return 1;
        }
        serverSockets.push_back(serverSocket);
        socketServerMap[serverSocket] = &servers[i];
    }

    return 0;
}

int createListeningSocket(std::string host, int port) {
    // Attach the socket to the port 
    // struct addrinfo {
    //     int              ai_flags;       // Options for getaddrinfo (e.g., AI_PASSIVE)
    //     int              ai_family;      // Address family (AF_INET, AF_INET6, AF_UNSPEC) 
    //     int              ai_socktype;    // Socket type (SOCK_STREAM, SOCK_DGRAM)
    //     int              ai_protocol;    // Protocol (TCP = 6, UDP = 17, usually 0 for auto)
    //     socklen_t        ai_addrlen;     // Length of ai_addr
    //     struct sockaddr *ai_addr;        // Pointer to actual address (sockaddr_in)
    //     char            *ai_canonname;   // Canonical name for hostname (if requested)
    //     struct addrinfo *ai_next;        // Pointer to next addrinfo in linked list
    // };
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;      // Only allow binding to local IP addresses
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP
    std::cout << GREEN << "Creating listening socket on " << port << RESET << std::endl;
    int status = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res);
    if ((status) != 0) { 
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return -1;
    }

    // Create socket 
    // AF_INET for IPv4, SOCK_STREAM for TCP
    // Server server; 
    int serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (serverSocket < 0) {
        std::cerr << RED << "Error creating socket" << RESET << std::endl;
        return -1;
    }

    // Set SO_REUSEADDR to allow quick reuse of the port
    // Set opt to 1 to enable the option
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind the socket to the specified IP/port
    if(bind(serverSocket, res->ai_addr, res->ai_addrlen) < 0) {
        std::cerr << RED << "Error binding socket" << RESET << std::endl;
        freeaddrinfo(res);
        close(serverSocket);
        return -1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0) {
        std::cerr << RED << "Error listening on socket" << RESET << std::endl;
        freeaddrinfo(res);
        close(serverSocket);
        return -1;
    }

    // Non-blocking mode for server socket
    // accept() uses serverSocket to wait for connection
    // If serverSocket is blocking, accept() will freeze the whole server
    // Retrieve current flag of the socket and 0 means does not change the flags
    int flags = fcntl(serverSocket, F_GETFL, 0);
    // preserve existing flags and add O_NONBLOCK flag 
    fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK);

    freeaddrinfo(res);
    return serverSocket;
}

void createPollFds(const std::vector<int>& serverSockets, std::vector<pollfd>& fds) {
    for (size_t i = 0; i < serverSockets.size(); ++i) {
        pollfd server_fd;
        server_fd.fd = serverSockets[i];
        server_fd.events = POLLIN; // reading data
        fds.push_back(server_fd);
    }
}