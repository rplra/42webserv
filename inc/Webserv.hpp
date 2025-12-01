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
#include <csignal>
#include <dirent.h>

class Request; // forward declaration

// Socket handling
int         createListeningSockets(std::string host, int port);
void        createPollFds(const std::vector<int>& serverSockets, std::vector<pollfd>& fds);
int         handleRequest(int serverSocket, std::vector<pollfd>& fds, std::map<int, int>& clientServerMap);

// utilities
std::string readFile(const std::string& filePath);
void        closeAllFd(std::vector<pollfd>& fds);
std::string        sendResponse(std::string filePath, int statusCode);
std::string        sendRedirectResponse(const std::string& redirectPath, int statusCode); 
void        closeClient(size_t& i, size_t& fds_count, std::vector<pollfd>& fds, std::map<int, Request>& clientRequests, std::map<int, int>& clientServerMap);

#endif