#ifndef __CONFIG_HPP__
# define __CONFIG_HPP__

# include <fstream> 
# include <sstream> 
# include "Webserv.hpp"

enum e_common_directive
{
	ROOT,
	INDEX,
	AUTOINDEX,
	ERROR_PAGE,
	CLIENT_MAX_BODY_SIZE,
};

enum e_global_scope
{
	SERVER
	/* plus common directives */
};

enum e_server_scope
{
	LISTEN,
	SERVER_NAME,
	LOCATION
	/* ... plus common directives */
};

enum e_location_scope
{
	CGI_HANDLER,		// not for global scope
	ALLOWED_METHODS,	//allowed_methods
	UPLOAD_STORE,		// upload path
	RETURN				// redirect path
	/* ... plus common directives */
};

// location should be a struct since it's a pure data container (no behaviour)
struct Location
{
	std::string					path;
	std::string 				root;
	std::string 				index;
	bool						autoindex;
	size_t						client_max_body_size;
	std::map<int, std::string>	error_pages;

	std::vector<std::string> 	allowed_methods;
	// later : cgi externsion -> cgi path
	// later : upload path
	// later : redirect path
};

// server contains data and behaviour (parsing, etc)
class Server
{
	public:
		Server();
		~Server();

		// getters : getHost(), getPort(), getRoot(), getIndex() 
		// helpers : matchLocation(), isAllowedMethods()

	private:
		std::string					_host;					// “127.0.0.1”
		int							_port;					// 8080
		std::vector<std::string>	_server_names;			// ["localhost", "google.com"]

		std::string					_root;					// default server root
		std::string					_index;					// default index
		bool						_autoindex;				// server-wide default
		size_t						_client_max_body_size;
		std::map<int, std::string>	_error_pages;

		std::vector<Location>		_locations;

		// methods : parser funcs, etc

};

void	parseConfig(char **av);

#endif