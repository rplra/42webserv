#pragma once
#ifndef __WEBSERV_HPP__
#define __WEBSERV_HPP__

// lib
#include <fstream>
#include <sstream>	
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <ctime>
#include <exception>
#include <stdexcept>

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
# include <limits.h>		// PATH_MAX

// headers or forward declaration?
#include "Macros.hpp"
#include "Config.hpp"
// #include "ConfigParse.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ServerManager.hpp"
#include "Client.hpp"
#include "Utils.hpp"
#include "Cookie.hpp"
// #include "Debug.hpp"

// class Config;
// class Server;
// class ServerManager;
// class Client;
// class Request;
// class Response;
// class Utils;

extern int g_signal;

#endif