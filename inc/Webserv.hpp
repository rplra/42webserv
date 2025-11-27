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
#include "ConfigParse.hpp"

#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/stat.h>

// class Server; // forward declaration
class Request; // forward declaration

// Socket handling
int         handleRequest(int serverSocket, std::vector<pollfd>& fds);
// int         handleRequest(int serverSocket, Server& server, Request& req);
// Location*   getBestMatchingLocation(const std::string& requestPath, const Server& server);
bool        isDirectory(const std::string& path);
bool        isFile(const std::string& path);
// std::string getErrorPagePath(int errorCode, const Server& server);

#endif