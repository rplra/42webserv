#include "Config.hpp"

Server::Server() 
:
	_host(),
	_port(),
	_server_names(),
	_root(),
	_index(),
	_autoindex(false),
	_client_max_body_size(0),
	_error_pages(),
	_locations()
{};

// Server::~Server(){}

const std::string& Server::getHost() const {
	return _host;
}

int Server::getPort() const {
	return _port;
}

const std::vector<std::string>& Server::getServerNames() const {
	return _server_names;
}

const std::string& Server::getRoot() const {
	return _root;
}

const std::string& Server::getIndex() const {
	return _index;
}

bool Server::getAutoindex() const {
	return _autoindex;
}

size_t Server::getClientMaxBodySize() const {
	return _client_max_body_size;
}

/* 
	if server->_root && location->_root.empty, simply use server block root and append to path
		- full path = root + path
	if location (always check alias first, alias takes precedence over root)
		- has ALIAS : remove prefix, root = location->alias, full path = root + path
		- has ROOT 	: keep prefix, root = location->root, full path = root +  prefix + path

	eg alias
	location /form { alias www/html }
	request = /form/form.html
	full path = www/html/form.html

	eg root
	location /asset/ { root www }
	request = /asset/img/cat.png
	full path = www/asset/img/cat.png
*/
std::string	Server::getFullPath(const Request& request) const
{
	const Location* location = getMatchingLocation(request.getPath());
	std::string path = request.getPath();
	std::string root = _root;

	/* debug */std::cout << YELLOW << "> ROUTING: initial req path: " << RESET << path << std::endl;

	// check alias first (precedence) and then root
	if (location)
	{
		if (!location->_alias.empty())
		{
			root = location->_alias;
			/* debug */std::cout << YELLOW << "> ROUTING: using alias: " << RESET << root << std::endl;
			
			size_t loc_len = location->_path.length();
			if (path.compare(0, loc_len, location->_path) == 0)
				path = path.substr(loc_len);
			if (path.empty() || path[0] != '/')
				path = "/" + path;
		}
		else if (!location->_root.empty())
		{
			root = location->_root;
			/* debug */std::cout << YELLOW << "> ROUTING: using location root: " << RESET << root << std::endl;
		}
	}

	return (normalizePath(root + path));
}

std::vector<Location>& Server::getLocations() {
	return _locations;
}

const std::vector<Location>& Server::getLocations() const {
	return _locations;
}

const std::string Server::getErrorPagePath(int errorCode) const {
	std::map<int, std::string>::const_iterator it = _error_pages.find(errorCode);
	std::map<int, std::string>::const_iterator default_it = _error_pages.find(404);

	if (it != _error_pages.end()) {
		return it->second;
	}

	return default_it->second;
}

/* 
	match by longest prefix match (location blocks can have the same initial prefix)
	location /images/
	location /images/cat > this will be the best match

	** always remove trailing slash for location prefix, 
	as the location config doesnt consider strictly on trailing slash since this func will always trim it
	** only alias need to strictly have trailing slash
*/
const Location* Server::getMatchingLocation(const std::string& requestPath) const {
	const Location* best_match = NULL;
	size_t best_len = 0;

	/* debug */std::cout << PINK << "> SVR: matching for req: " << RESET << requestPath << std::endl;
	for (size_t i = 0; i < _locations.size(); ++i) {
		const Location& loc = _locations[i];
		std::string loc_path = loc._path;
		
		if (loc_path.length() > 1 && loc_path[loc_path.length() - 1] == '/')
			loc_path = loc_path.substr(0, loc_path.length() - 1);

		size_t len = loc_path.length();
		if (requestPath.compare(0, len, loc_path) == 0 
			&& (requestPath.length() == len || requestPath[len] == '/')) {
			if (len > best_len) {
				best_len = len;
				best_match = &loc;
			}
		}
	}
	return best_match;
}

void Server::setPort(int port) {
	_port = port;
}

void Server::setHost(const std::string& host) {
	_host = host;
}

void Server::addServerName(const std::string& server_name) {
	_server_names.push_back(server_name);
}

void Server::setRoot(const std::string& root) {
	_root = root;
}

void Server::setIndex(const std::string& index) {
	_index = index;
}

void Server::setAutoindex(bool autoindex) {
	_autoindex = autoindex;
}

void Server::setClientMaxBodySize(size_t size) {
	_client_max_body_size = size;
}

void Server::addErrorPage(int errorCode, const std::string& path) {
    // std::cout << GREEN << "Adding error page for code " << errorCode << " with path: " << path << RESET << std::endl;
	_error_pages[errorCode] = path;
}

void Server::addLocation(const Location& location) {
	_locations.push_back(location);
}


Config::Config() : _servers(), _port_map()
{
}

std::vector<Server>& Config::getServers()
{
	return (_servers);
}

const std::vector<Server>& Config::getServers() const
{
	return (_servers);
}

const std::vector<Server*>& Config::getServerOnPort(int port) const
{
	static const std::vector<Server*> empty;
	std::map<int, std::vector<Server*> > ::const_iterator it = _port_map.find(port);
	if (it != _port_map.end())
		return (it->second);
	return (empty);
}

std::vector<Server*>& Config::getServerOnPort(int port)
{
	return (_port_map[port]);
}
