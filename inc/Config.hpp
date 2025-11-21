#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include "Webserv.hpp"

// location should be a struct since it's a pure data container (no behaviour)
// this is where HTTP and Config bridge (http depends entirely on Config's Location)
// the fields needed by http else http can't function
struct Location
{
	std::string					path;
	std::string					root;
	std::string					index;
	bool						autoindex;
	std::vector<std::string> 	allowed_methods;
	// later : cgi externsion -> cgi path
	// later : upload path
	// later : redirect path + code(?)
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

	// helpers
	// HTTP use    : matchLocation() - returns Location* based on longest prefix match
	//			   : isAllowedMethod()
	// Network use : isMatchesPort() (optional) - does this server listen on given port?
	//			   : isMatchesHost() (optional) - does this server match the Host header?

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
};

#endif