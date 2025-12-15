#include "Webserv.hpp"

int handleRequest(int serverSocket, std::vector<pollfd>& fds, std::map<int, int>& clientServerMap) {
    // Accept a connection
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket < 0) 
        return 0;
    
    pollfd client_fd;
    client_fd.fd = clientSocket;
    client_fd.events = POLLIN; // reading and writing data
    fds.push_back(client_fd);
    
    // Non-blocking mode for client socket
    // recv() uses clientSocket to read data
    // If clientSocket is blocking, recv() will freeze the whole server
    // Retrieve current flags of the socket and set O_NONBLOCK
    int flags = fcntl(clientSocket, F_GETFL, 0);
    fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);
    
    clientServerMap[clientSocket] = serverSocket;
    std::cout << GREEN << "Mapped client fd: " << clientSocket << " to server fd: " << serverSocket << RESET << std::endl;
    return 0;
}

std::string checkMaxBodySize(const Server* server, size_t totalReceived, size_t maxBodySize, 
    std::string fullPath, const Location* locPath) {

    std::cout << GREEN << "Max body size: " << RESET << maxBodySize << std::endl; // DEBUG
    std::cout << GREEN << "Total received: " << RESET << totalReceived << std::endl; // DEBUG

    if (totalReceived >= maxBodySize) {
        std::cerr << RED << "Error: Request body too large" << RESET << std::endl;
        std::string serverErrorPath = server->getRoot() + "/" + server->getErrorPagePath(500);
        if (locPath && locPath->error_pages.find(500) != locPath->error_pages.end()) {
            serverErrorPath = fullPath + "/" + locPath->error_pages.at(500);
        }
        return createResponse(serverErrorPath, 500);
    }
    return "";
}