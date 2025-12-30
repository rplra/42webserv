#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "Webserv.hpp"

//			main
void		checkArgument(int ac);

//			config
std::string	trimStringTail(const std::string &str, char c);
std::string	trimStringHead(const std::string &str, char c);
std::string	normalizePath(const std::string& path);

//			http
std::string toLower(const std::string& s);
std::string trim(const std::string& s);
std::string	urlDecode(const std::string& s);

//			generic
bool		isDirectory(const std::string& path);
bool		isFile(const std::string& path);

void 		handleSignal(int signum);

#endif