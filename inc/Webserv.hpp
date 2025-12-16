#pragma once
#ifndef __WEBSERV_HPP__
#define __WEBSERV_HPP__

// lib
#include <exception>
#include <iostream>
#include <cstring>
#include <string>

// stl
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>

// sys
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <sys/stat.h>
// #include <sys/time.h>
// #include <signal.h> 

// net
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>

// headers
#include "Request.hpp"
#include "Macros.hpp"
#include "Utils.hpp"
#include "Config.hpp"

#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

class Request; // forward declaration

// Socket handling
int     createAllListeningSockets(const std::vector<Server>& servers, 
            std::vector<int>& serverSockets, std::map<int, const Server*>& socketServerMap);
int     createListeningSocket(std::string host, int port);
void    createPollFds(const std::vector<int>& serverSockets, std::vector<pollfd>& fds);

// Request handling
int         handleRequest(int serverSocket, std::vector<pollfd>& fds, std::map<int, int>& clientServerMap);
std::string checkMaxBodySize(const Server* server, size_t totalReceived, size_t maxBodySize, 
    std::string fullPath, const Location* locPath);

// Bridge function between Request and Response handling
int         handleResponse(std::map<int, int>& clientServerMap, std::map<int, Request>& clientRequests, 
                std::map<int, std::string>& clientSendBuffers, std::map<int, const Server*>& socketServerMap,
                size_t& i, size_t& fds_count, std::vector<pollfd>& fds);
std::string sendData(const Server* clientServer, const Request& client, const Location* locPath, size_t totalReceived);

// Response handling
std::string readFile(const std::string& filePath);
std::string createResponse(std::string filePath, int statusCode);
std::string createRedirectResponse(const std::string& redirectPath, int statusCode); 
std::string generateAutoindexPage(const std::string& dirPath, const std::string& requestPath); 
void sendResponse(size_t& i, size_t& fds_count, std::vector<pollfd>& fds, 
    std::map<int, Request>& clientRequests, 
    std::map<int, int>& clientServerMap, 
    std::map<int, std::string>& clientSendBuffers);
std::string createResponseFromCGI(const std::string& cgiResponse);

#endif