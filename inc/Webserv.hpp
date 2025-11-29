#pragma once
#ifndef __WEBSERV_HPP__
#define __WEBSERV_HPP__

// lib
#include <dirent.h> // posix dir-handling API
#include <exception>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <string>
#include <ctime>

// stl
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>

// sys
// #include <sys/types.h>
// #include <sys/wait.h>
#include <sys/stat.h> // S_ISREG
// #include <sys/time.h>
// #include <signal.h> 

// net
#include <sys/socket.h>

// headers
#include "Macros.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Utils.hpp"


#endif