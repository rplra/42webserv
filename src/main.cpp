#include "Webserv.hpp"

int g_signal;

std::string checkMaxBodySize(const Server* server, size_t totalReceived, size_t maxBodySize, std::string fullPath, const Location* locPath) {
    std::cout << GREEN << "Max body size: " << RESET << maxBodySize << std::endl; // DEBUG
    std::cout << GREEN << "Total received: " << RESET << totalReceived << std::endl; // DEBUG
    if (totalReceived >= maxBodySize) {
        std::cerr << RED << "Error: Request body too large" << RESET << std::endl;
        std::string serverErrorPath = server->getRoot() + "/" + server->getErrorPagePath(500);
        if (locPath && locPath->error_pages.find(500) != locPath->error_pages.end()) {
            serverErrorPath = fullPath + "/" + locPath->error_pages.at(500);
        }
        return sendResponse(serverErrorPath, 500);
    }
    return "";
}

std::string generateAutoindexPage(const std::string& dirPath, const std::string& requestPath) {
    std::string body = "<html><head><title>Index of " + requestPath + "</title></head><body>";
    body += "<h1>Index of " + requestPath + "</h1><ul>";

    DIR* dir = opendir(dirPath.c_str());
    if (dir == nullptr) {
        std::cerr << RED << "Error: Unable to open directory for autoindex" << RESET << std::endl;
        return sendResponse("var/error.html", 500);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == ".") continue; // skip current directory
        body += "<li><a href=\"" + requestPath;
        if (requestPath.back() != '/')
            body += "/";
        body += name + "\">" + name + "</a></li>";
    }
    closedir(dir);

    body += "</ul></body></html>";

    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << body;

    return response.str();
}

std::string sendData(const Server* clientServer, const Request& client, const Location* locPath, size_t totalReceived) {
    std::string fullPath = clientServer->getRoot() + client.getPath();
    if (locPath && locPath->root != "")
        fullPath = locPath->root + client.getPath();

    std::cout << GREEN << "Full path before checks: " << RESET << fullPath << std::endl;
    std::cout << GREEN << "Directory ? " << RESET << clientServer->isDirectory(fullPath) << std::endl;

    // check client_max_body_size in location first, then server
    if (locPath && locPath->client_max_body_size > 0) {
        std::string result = checkMaxBodySize(clientServer, totalReceived, locPath->client_max_body_size, fullPath, locPath);
        if (result != "")
            return result;
    }
    else {
        std::string result = checkMaxBodySize(clientServer, totalReceived, clientServer->getClientMaxBodySize(), "", nullptr);
        if (result != "")
            return result;
    }

    if (clientServer->getIndex() != "" && client.getPath() == "/") {
        std::string fullPath = clientServer->getRoot() + "/" + clientServer->getIndex();
        return sendResponse(fullPath, 200);
    }
    else if (clientServer->isDirectory(fullPath)) {
        if (locPath && locPath->index != "") {
            std::cout << GREEN << "Full path to resource: " << RESET << fullPath << std::endl;
            if (fullPath[fullPath.length() - 1] != '/')
                fullPath += "/";
            std::string indexPath = fullPath + locPath->index;
            return sendResponse(indexPath, 200);
        }
        else if (locPath && locPath->autoindex) {
            return generateAutoindexPage(fullPath, client.getPath());
        }
        else {
            if (locPath && locPath->error_pages.find(403) != locPath->error_pages.end()) {
                std::string errorPagePath = fullPath + "/" + locPath->error_pages.at(403);             
                std::cout << GREEN << "error Page path: " << RESET << errorPagePath << std::endl;
                return sendResponse(errorPagePath, 403);

            }
            std::string errorPagePath = clientServer->getRoot() + "/" + clientServer->getErrorPagePath(403);
            return sendResponse(errorPagePath, 403);
        }
    }
    else if (locPath && locPath->redirect.size() > 0) {
        std::map<int, std::string>::const_iterator it = locPath->redirect.begin();
        int redirectCode = it->first;
        std::string redirectPath = it->second;
        std::cout << GREEN << "Redirecting to: " << RESET << redirectPath << " with code " << redirectCode << std::endl;
        return sendRedirectResponse(redirectPath, redirectCode);
    }
    else if (locPath && clientServer->isFile(fullPath)) {
        std::cout << GREEN << "Full path to resource: " << RESET << fullPath << std::endl;
        return sendResponse(fullPath, 200);
    }
    else {
        std::string errorPagePath = clientServer->getRoot() + "/" + clientServer->getErrorPagePath(404);
        return sendResponse(errorPagePath, 404);
    }

    return "";
}

std::string sendRedirectResponse(const std::string& redirectPath, int statusCode) {
    std::string response;
    response += "HTTP/1.1 " + std::to_string(statusCode) + " Moved Permanently\r\n";
    response += "Location: " + redirectPath + "\r\n";
    response += "Content-Length: 0\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";

    return response;
}

void handleSignal(int signum) {
    g_signal = 0;
    std::cout << RED << "\nSignal " << signum << " received, shutting down server..." << RESET << std::endl;
}

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
    for (size_t i = 0; i < servers.size(); ++i) {
        int serverSocket = createListeningSockets(servers[i].getHost(), servers[i].getPort());
        if (serverSocket < 0) {
            std::cerr << RED << "Failed to create listening socket for server on port " << servers[i].getPort() << RESET << std::endl;
            return 1;
        }
        serverSockets.push_back(serverSocket);
        socketServerMap[serverSocket] = &servers[i];
    }

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
                continue; // interrupted by signal, continue polling
            else {
                std::cerr << RED << "Error in poll()" << RESET << std::endl;
                closeAllFd(fds);
                return 1;
            }
        }

        std::cout << GREEN << "Server Event: " << RESET << fds[0].revents << std::endl; // DEBUG
        size_t fds_count = fds.size();
        for (size_t i = 0; i < fds_count; ++i) {

            if (fds[i].revents != 0) {
                std::cout << "fd " << fds[i].fd << " revents: " << fds[i].revents << std::endl;
            }
            
            if (fds[i].revents & POLLIN) {
                // Listen for new incoming connections
                if (std::find(serverSockets.begin(), serverSockets.end(), fds[i].fd) != serverSockets.end()) {
                    handleRequest(fds[i].fd, fds, clientServerMap);
                } else {
                    const Server*   clientServer = socketServerMap[clientServerMap[fds[i].fd]];
                    Request&        request = clientRequests[fds[i].fd];
                    std::string     req;
                    char            buffer[1024];
                    size_t          totalReceived = 0;
                    ssize_t         bytes = recv(fds[i].fd, buffer, sizeof(buffer), 0);
    
                    if (bytes == 0) {
                        // Connection closed by client
                        std::cout << YELLOW << "Client disconnected (fd " << fds[i].fd << ")" << RESET << std::endl;
                        closeClient(i, fds_count, fds, clientRequests, clientServerMap);
                        continue; // Skip to next fd
                    } else if (bytes < 0) {
                        std::cerr << RED << "recv() error on fd " << fds[i].fd << RESET << std::endl;
                        closeClient(i, fds_count, fds, clientRequests, clientServerMap);
                        continue;
                    } else {
                        totalReceived += bytes;
                        req.append(buffer, bytes);
                        std::cout << GREEN << "Buffer: " << RESET << req << std::endl; // DEBUG
                        request.readRequest(req);

                        // Ensure the entire request is parsed before proceeding
                        if (request.isParseComplete()) {
                            std::cout << GREEN << "Location for path: " << request.getPath() << RESET << std::endl;
                            const Location* locPath = clientServer->bestMatchingLocation(request.getPath());
                            std::cout << GREEN << "Best matching location: " << RESET << (locPath ? locPath->path : "None") << std::endl;
                            std::cout << GREEN << "index: " << RESET << (locPath ? locPath->index : "None") << std::endl;
                
                            if (locPath) {
                                std::string path = sendData(clientServer, request, locPath, totalReceived);
                                clientSendBuffers[fds[i].fd] = path;
                                fds[i].events |= POLLOUT; // enable write event
                            }
                        }                       
                    }
                }
            }             

            // Send response to client
            if ((fds[i].revents & POLLOUT) && !clientSendBuffers[fds[i].fd].empty()) {
                std::string& sendBuffer = clientSendBuffers[fds[i].fd];
                // std::cout << GREEN << "Send Buffer: " << RESET << sendBuffer << std::endl; // DEBUG
                ssize_t bytesSent = send(fds[i].fd, sendBuffer.c_str(), sendBuffer.size(), 0);
                if (bytesSent > 0) {
                    sendBuffer.erase(0, bytesSent);
                    if (sendBuffer.empty()) {
                        fds[i].events &= ~POLLOUT; // remove write event
                        closeClient(i, fds_count, fds, clientRequests, clientServerMap);
                        std::cout << GREEN << "i: " << RESET << i << std::endl;
                        std::cout << GREEN << "fds_count: " << RESET << fds_count << std::endl;
                    }
                } else if (bytesSent < 0) {
                    std::cerr << RED << "send() error on fd " << fds[i].fd << RESET << std::endl;
                    closeClient(i, fds_count, fds, clientRequests, clientServerMap);
                } else {
                    // bytesSent == 0, connection closed
                    std::cerr << RED << "Connection closed while sending on fd " << fds[i].fd << RESET << std::endl;
                    closeClient(i, fds_count, fds, clientRequests, clientServerMap);
                }
            }
        }
    }

    closeAllFd(fds);
    return 0;
}

void closeClient(size_t& i, size_t& fds_count, std::vector<pollfd>& fds, std::map<int, 
    Request>& clientRequests, std::map<int, int>& clientServerMap) {

    close(fds[i].fd);
    clientRequests.erase(fds[i].fd);
    clientServerMap.erase(fds[i].fd);
    fds.erase(fds.begin() + i);

    // adjust index after erasing element as the remaining elements shift left 
    i--;
    fds_count--;
}