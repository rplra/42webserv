#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "Webserv.hpp"

void		checkArgument(int ac);

// parsing
std::string	trimStringTail(const std::string &str, char c);
std::string	trimStringHead(const std::string &str, char c);

#endif