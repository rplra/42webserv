#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "Request.hpp"
#include <csignal>

class Request; // forward declaration
extern int g_signal;

// http
void		checkArgument(int ac);

// http
// std::string	normalizePath(const std::string& s);
std::string toLower(const std::string& s);
std::string trim(const std::string& s);

// close 
void        closeAllFd(std::vector<pollfd>& fds);
void        closeClient(size_t& i, size_t& fds_count, std::vector<pollfd>& fds, 
    std::map<int, Request>& clientRequests, std::map<int, int>& clientServerMap);
void handleSignal(int signum);

#endif