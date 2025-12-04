#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include "Webserv.hpp"

# define CLIENT_MAX_BODY	1000

// location should be a struct since it's a pure data container (no behaviour)
// this is where HTTP and Config bridge (http depends entirely on Config's Location)
// the fields needed by http else http can't function
struct Location
{
	std::string					_path;			//location path
	std::string					_root;
	std::string					_index;
	bool						_autoindex;
	size_t						_client_max_body_size;
	std::map<int, std::string>	_error_pages;
	std::vector<std::string> 	_allowed_methods;

	std::string					_upload_path;
	std::map<int, std::string>	_cgi;			//cgi extension type, cgi path
	std::map<int, std::string>	_redirect;		//return code, redirect path
};

struct errCheck
{
	int							line_count;
	std::string					keyword;
};

// server contains data and behaviour (parsing, etc)
// if parsing bloats this Server class, can create another seperate ConfigParser class
// Network and HTTP depend on these fields to route requests
class Server
{
public:
	Server();
	~Server();

	void		parseServer(std::ifstream &inFile);
	static void	errorCheckServer(std::ifstream &inFile);

	void		printErrorPage();
	void		printServer();

	void		assignMapContainer(std::map<int, std::string> &data, std::istringstream &iss);
	void		assignVecContainer(std::vector<std::string> &data, std::istringstream &iss);

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
	std::string					_host;					// ip addr
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

	// error check use
	errCheck					_check;

	// methods : parser funcs, etc
	template <typename T>
	bool		checkCommonDirective(std::string str, std::istringstream &iss, T &data);
	template <typename T>
	void		getCommonDirective(std::size_t code, std::istringstream &iss, T &data);

	bool		handleServerDirective(std::string str, std::istringstream &iss, std::ifstream &inFile);
	bool		handleLocationDirective(std::string str, std::istringstream &iss, Location& data);
	
	void		getServerDirective(std::size_t code, std::istringstream &iss, std::ifstream &inFile);
	void		getLocationDirective(std::size_t code, std::istringstream &iss, Location &data);
	void		getListen(std::istringstream &iss);
	void		initLocation(Location &obj);

	void		parseLocation(std::ifstream &inFile, std::istringstream &iss);

	// parser: error checks
	static int		countArgs(std::istringstream &iss);
	static bool		ignoreKeyword(std::string &word);

	static bool		errorServerDirective(std::string &str, std::istringstream &iss, std::ifstream &inFile);
	static bool		errorCommonDirective(std::string &str, std::istringstream &iss);
	// static bool		errorCheckListen(std::istringstream &iss);
	static void		checkCommonArgCount(size_t code, std::istringstream &iss);
	static void		checkServerArgCount(size_t code, std::istringstream &iss, std::ifstream &inFile);
	static void		checkLocationArgCount(size_t code, std::istringstream &iss);

	static bool		errorLocationArgCount(std::istringstream &iss, std::ifstream &inFile);
	static bool		errorLocationDirective(std::string str, std::istringstream &iss);
};

// this is MAIN BRIDGE btw Network + Config 
// Network depends on Config to know which Server obj's exist, which port they are on,
// and which servers share the same port (virtual hosts)
// HTTP depends on Server obj inside Config for routing + request handling
class Config
{
public:
	void	parseConfig(char **av);

	// Config provide these to Network / HTTP after parsing the config file
	
	// needed by Network, rarely used by HTTP
	const std::vector<Server>& getServers() const;

	// critical for Network
	// Network asks : "which server are listening on this port?"
	// Config must return the correct vector of pointers for that port
	const std::vector<Server*> getServerOnPort(int port) const;

private: 
	void	startParser(std::ifstream &inFile);
	void	printAllServer();
	bool	errorCheckConfig(std::ifstream &inFile);

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

	// helper functions
};

#endif