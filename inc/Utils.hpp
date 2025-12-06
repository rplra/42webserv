#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "Webserv.hpp"

// #include "Request.hpp"
// #include <csignal>

// class Request;
// extern int g_signal;

// main
void		checkArgument(int ac);

// // parsing
// std::string	trimStringTail(const std::string &str, char c);
// std::string	trimStringHead(const std::string &str, char c);

// // http
// // std::string	normalizePath(const std::string& s);
std::string toLower(const std::string& s);
std::string trim(const std::string& s);

// // generic
bool		isDirectory(const std::string& path);
bool		isFile(const std::string& path);

// // close 
// void        closeAllFd(std::vector<pollfd>& fds);
// void        closeClient(size_t& i, size_t& fds_count, std::vector<pollfd>& fds, 
//     std::map<int, Request>& clientRequests, std::map<int, int>& clientServerMap);
void handleSignal(int signum);

#endif