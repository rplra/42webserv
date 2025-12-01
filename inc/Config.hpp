#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include "Webserv.hpp"
# include <fstream> 
# include <sstream> 

enum e_common_directive
{
	NONE_COMMON,
	ROOT,
	INDEX,
	AUTOINDEX,
	ERROR_PAGE,
	CLIENT_MAX_BODY_SIZE,
};

enum e_global_scope
{
	NONE_GLOBAL,
	SERVER
	/* plus common directives */
};

enum e_server_scope
{
	NONE_SERVER,
	LISTEN,
	SERVER_NAME,
	LOCATION
	/* ... plus common directives */
};

enum e_location_scope
{
	NONE_LOCATION,
	CGI_HANDLER,		// not for global scope
	ALLOWED_METHODS,	//allowed_methods
	UPLOAD_STORE,		// upload path
	RETURN				// redirect path
	/* ... plus common directives */
};

// location should be a struct since it's a pure data container (no behaviour)
// this is where HTTP and Config bridge (http depends entirely on Config's Location)
// the fields needed by http else http can't function
struct Location
{
	std::string					path;			//location path
	std::string					root;
	std::string					index;
	bool						autoindex;
	size_t						client_max_body_size;
	std::map<int, std::string>	error_pages;
	std::vector<std::string> 	allowed_methods;

	std::map<int, std::string>	cgi;			//cgi extension type, cgi path
	std::string					upload_path;
	std::map<int, std::string>	redirect;		//return code, redirect path
};

// server contains data and behaviour (parsing, etc)
// if parsing bloats this Server class, can create another seperate ConfigParser class
// Network and HTTP depend on these fields to route requests
class Server
{
public:
	Server();
	~Server();

	// getters - these method names must align for ALL otherwise integration fails
	// HTTP use    : getRoot(), getIndex(), getLocations(), getErrorPages()
	// Network use : getHost(), getPort(),  getServerNames()
	const std::string&					getHost() const;
	int									getPort() const;
	const std::vector<std::string>&		getServerNames() const;
	const std::string&					getRoot() const;
	const std::string&					getIndex() const;
	bool								getAutoindex() const;
	size_t								getClientMaxBodySize() const;
	std::vector<Location>&				getLocations() ;
	const std::string					getErrorPagePath(int errorCode) const;
	const Location*						bestMatchingLocation(const std::string& requestPath) const;

	// setters
	void								setPort(int port);
	void								setHost(const std::string& host);
	void								addServerName(const std::string& server_name);
	void								setRoot(const std::string& root);
	void								setIndex(const std::string& index);
	void								setAutoindex(bool autoindex);
	void								setClientMaxBodySize(size_t size);
	void								addErrorPage(int errorCode, const std::string& path);
	void 								addLocation(const Location& location);

	// helpers
	// HTTP use    : matchLocation() - returns Location* based on longest prefix match
	//			   : isAllowedMethod()
	// Network use : isMatchesPort() (optional) - does this server listen on given port?
	//			   : isMatchesHost() (optional) - does this server match the Host header?
	bool								isDirectory(const std::string& path) const;
	bool								isFile(const std::string& path) const;

// this is where both network and config MUST ALIGN in terms of what DATA TYPE and FIELDS to have
// network must let config know what fields it expects (if config doesnt have, then it fails to build)
private:
	// Config <-> Network alignment : Network needs exact types to bind sockets
	std::string					_host;					
	int							_port;					// string or int?
	std::vector<std::string>	_server_names;			// string or vector?
	
	// Server-Wide Default (used by HTTP when location-specific value not present)
	std::string					_root;					//
	std::string					_index;					//
	bool						_autoindex;				//
	size_t						_client_max_body_size;

	// HTTP use
	std::map<int, std::string>	_error_pages;
	std::vector<Location>		_locations;

	// methods : parser funcs, etc
};

// this is MAIN BRIDGE btw Network + Config 
// Network depends on Config to know which Server obj's exist, which port they are on,
// and which servers share the same port (virtual hosts)
// HTTP depends on Server obj inside Config for routing + request handling
class Config
{
public:
	Config();
	~Config();

	void	parseConfig(char **av);

	// Config provide these to Network / HTTP after parsing the config file
	// needed by Network, rarely used by HTTP
	const std::vector<Server>& getServers() const;

	// critical for Network
	// Network asks : "which server are listening on this port?"
	// Config must return the correct vector of pointers for that port
	const std::vector<Server*> getServerOnPort(int port) const;

private: 
	// CONFIG creates Server Objs after parsing
	// Network depends on these objs to bind sockets
	// store in vector? or other type of Container? (usually vector is fine)
	std::vector<Server>						_servers;

	// Port Map : essential for Network
	// - Network will bind 1 socket per port
	// - multiple servers may share same port (virtual hosting)
	// - Network must known which servers share a port to pick the correct one based on Host header
	// Map<key, value> container is recommended; key = port, value = vector<Server*> pointing to servers in _servers
	std::map<int, std::vector<Server*> >	_port_map;

	int 						_global_scope;
	int							_server_scope;
	int							_location_scope;
	int							_common_directive;

	void						block_validation(std::string buffer);
	std::string					removeSemicolon(std::string str);
};

#endif