#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "Webserv.hpp"

// main
void			checkArgument(int ac);

// parsing
std::string	trimStringTail(const std::string &str, char c);
std::string	trimStringHead(const std::string &str, char c);

// http
// std::string	normalizePath(const std::string& s);
std::string toLower(const std::string& s);
std::string trim(const std::string& s);




#endif