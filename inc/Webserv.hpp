#pragma once
#ifndef __WEBSERV_HPP__
#define __WEBSERV_HPP__

// lib
#include <exception>
#include <fstream>
#include <sstream>	
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <ctime>

// stl
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>

// sys
#include <dirent.h>			// opendir, readdir, closedir
#include <fcntl.h>			// open, fcntl(O_NONBLOCK)
#include <sys/stat.h>		// stat, S_ISREG, S_ISDIR
#include <sys/types.h>		// CGI: pid_t (process ID)
#include <sys/wait.h>		// CGI: wait(), waitpid()
#include <sys/time.h>		// struct timeval - used with socket timeouts
#include <signal.h> 		// signal, kill (handle SIGINT)
#include <unistd.h>			// read, write, close

// net
#include <sys/socket.h>		// socket, bind, listen, accept, send, recv, setsockopt
#include <netinet/in.h>		// struct sockaddr_in, htons, htonl
#include <netdb.h>			// gethostbyname, getaddrinfo, struct addrinfo
#include <poll.h>			// poll, struct pollfd

// headers
#include "Macros.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include "Config.hpp"
#include "ServerDebug.hpp"


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

#endif