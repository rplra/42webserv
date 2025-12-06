#ifndef __MACROS_HPP__
#define __MACROS_HPP__

#include <string>

/* 
	for c++, we avoid using #define (could be accidentally modified)
	because define behaves differently in C++ compared to C
	- use `const` instead to follow modern C++ approach
	- correct use is `constexpr` but it's only available for C++11 onwards 
 */

// colours
const std::string GREEN  = "\033[38;2;168;204;124m";
const std::string RED    = "\033[38;2;191;97;106m";
const std::string CYAN   = "\033[38;2;136;193;208m";
const std::string PURPLE = "\033[38;2;174;134;255m";
const std::string PINK   = "\033[38;2;224;147;217m";
const std::string YELLOW = "\033[38;2;255;214;2m";
const std::string ORANGE = "\033[38;2;255;135;0m";
const std::string GREY   = "\033[90m";
const std::string RESET  = "\033[0m";

// limits (should be from Config.client_max_xx_size - similar to NGINX)
// put here first
const size_t client_max_header_size	= 8192;		// 8 KB (default = 4 buffers x 8 KB = 32 KB /request)
const size_t client_max_body_size	= 1048576;	// 1 MB (default, can be increased - 10 MB etc)

// values
const size_t BUFFER_SIZE = 8192;

// error args
const std::string ERR_ARGFORMAT			= "Invalid argument. Usage: ./webserv [configuration file]";
const std::string ERR_FILEINVALID		= "Invalid file!";
const std::string ERR_FILEEMPTY			= "Empty file!";

// error - servermanager
const std::string ERR_POLL				= "Error in poll()";
const std::string ERR_CLIENTDISCONECT 	= "Client disconnected at fd: ";
const std::string ERR_RECVFD 			= "recv() error on fd ";

const std::string ERR_SERVERCONFIG		= "No servers configured";
const std::string ERR_GETADDRINFO		= "Getaddrinfo error: ";
const std::string ERR_CREATEALLSOCK		= "Failed to create listening socket for server on port ";
const std::string ERR_CREATESOCK		= "Error creating socket";
const std::string ERR_BINDSOCK			= "Error binding socket";
const std::string ERR_LISTENSOCK		= "Error listening on socket";

// http
enum HttpStatus
{
	HTTP_OK	= 200,
	HTTP_MOVED_PERMANENTLY = 301, // redirects
	HTTP_BAD_REQUEST = 400,
	HTTP_FORBIDDEN = 403,
	HTTP_NOT_FOUND = 404,
	HTTP_METHOD_NOT_ALLOWED = 405,
	HTTP_LENGTH_REQUIRED = 411,
	HTTP_PAYLOAD_TOO_LARGE = 413,
	HTTP_INTERNAL_SERVER_ERROR = 500,
	HTTP_NOT_IMPLEMENTED = 501,
	HTTP_BAD_GATEWAY = 502,
	HTTP_SERVICE_UNAVAILABLE = 503
};

#endif