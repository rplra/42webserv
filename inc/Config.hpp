#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include "Webserv.hpp"
class Request;

struct Location
{
	std::string					_path;
	std::string					_root;
	std::string					_index;
	bool						_autoindex;
	size_t						_client_max_body_size;
	std::map<int, std::string>	_error_pages;
	std::vector<std::string> 	_allowed_methods;

	std::string					_upload_path;
	std::map<std::string, std::string>	_cgi;			//cgi extension type, cgi path
	std::map<int, std::string>	_redirect;		//return code, redirect path
};


class Server
{
public:
	Server();
	~Server() {};

	const std::string&					getHost() const;
	int									getPort() const;
	const std::vector<std::string>&		getServerNames() const;
	const std::string&					getRoot() const;
	const std::string&					getIndex() const;
	bool								getAutoindex() const;
	size_t								getClientMaxBodySize() const;
	std::string							getFullPath(const Request& request) const;
	std::vector<Location>&				getLocations();
	const std::vector<Location>&		getLocations() const;
	const std::string					getErrorPagePath(int errorCode) const;
	const Location*						getMatchingLocation(const std::string& requestPath) const;

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

private:
	std::string					_host;
	int							_port;
	std::vector<std::string>	_server_names;
	
	// Server-Wide Default (used by HTTP when location-specific value not present)
	std::string					_root;
	std::string					_index;
	bool						_autoindex;
	size_t						_client_max_body_size;

	// HTTP use
	std::map<int, std::string>	_error_pages;
	std::vector<Location>		_locations;
};


class Config
{
public:
	Config();
	~Config() {};

	// NON CONST for parser to update this class while parsing
	std::vector<Server>& 			getServers();
	std::vector<Server*>&			getServerOnPort(int port);

	const std::vector<Server>& 		getServers() const;
	const std::vector<Server*>&		getServerOnPort(int port) const;

private: 
	std::vector<Server>						_servers;
	std::map<int, std::vector<Server*> >	_port_map;

};

#endif