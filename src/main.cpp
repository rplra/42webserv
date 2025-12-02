#include "Webserv.hpp"

int g_signal;

/* 
    HTTP Server 
    - a computer program that serves webpages to clients over the HTTP protocol     

    Flow of a server-side socket programming 
    1. socket() - create a socket
    2. bind() - bind the socket to an IP/port
    3. listen() - limits how many connections can wait before being accepted
    4. accept() - accept a connection
    5. recv() - receive data from a connection
    6. close() - close the connection

    1. set up server socket and get the port number from config file 
    2. receive request from client and store in a struct
    3. process the request and generate a response

    GET method - headers + blank line
    POST method - headers + blank line + body
*/
int main(int ac, char **av) {
    // signal handling for shutting down
    g_signal = 1;
    signal(SIGINT, handleSignal);

    checkArgument(ac);

    Config config;
	config.parseConfig(av);

    // create listening sockets for every servers 
    const std::vector<Server>& servers = config.getServers();
    if (servers.empty()) {
        std::cerr << RED << "No servers configured" << RESET << std::endl;
        return 1;
    }

    std::vector<int> serverSockets;
    std::map<int, const Server*> socketServerMap;
    if (createAllListeningSockets(servers, serverSockets, socketServerMap) != 0) 
        return 1;

    // Managing I/O multiplexing
    // In order to let the server run continuously, use poll() with infinite loop to monitor the server socket 
    // Reason of using std::vector is because we does not know how many clients will connect
    // Hence we can dynamically add client fds to the vector
    std::vector<pollfd> fds;
    createPollFds(serverSockets, fds);

    // check the client map with which server socket
    std::map<int, int> clientServerMap; 
    std::map<int, Request> clientRequests;
    std::map<int, std::string> clientSendBuffers;

    // Infinite loop to keep the server running 
    while (g_signal) {
        // Wait up to 5 sec for any fd in fds to be ready
        // select() uses fd_set which has limitation on max fds (1024) 
        // select() can also be used here but poll() is more scalable for a large number of fds 
        // as poll() does not have fd limitations
        int ready_count = poll(fds.data(), fds.size(), 5000);
        std::cout << GREEN << "Ready: " << RESET << ready_count << std::endl; // DEBUG
        if (ready_count < 0) {
            if (errno == EINTR) 
                continue; // interrupted by signal 
            else {
                std::cerr << RED << "Error in poll()" << RESET << std::endl;
                closeAllFd(fds);
                return 1;
            }
        }

        std::cout << GREEN << "Server Event: " << RESET << fds[0].revents << std::endl; // DEBUG
        size_t fds_count = fds.size();
        for (size_t i = 0; i < fds_count; ++i) {
        
            // Listen for new incoming connections
            if (fds[i].revents & POLLIN) {
                if (std::find(serverSockets.begin(), serverSockets.end(), fds[i].fd) != serverSockets.end()) {
                    handleRequest(fds[i].fd, fds, clientServerMap);
                } else {
                    int result = handleResponse(clientServerMap, clientRequests, clientSendBuffers, socketServerMap, i, fds_count, fds);
                    if (result != 0)
                        continue;
                }
            }             

            // Send response to client
            if ((fds[i].revents & POLLOUT) && !clientSendBuffers[fds[i].fd].empty()) {
                sendResponse(i, fds_count, fds, clientRequests, clientServerMap, clientSendBuffers);
            }
        }
    }

    closeAllFd(fds);
    return 0;
}