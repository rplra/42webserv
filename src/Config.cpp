#include "Config.hpp"

Server::Server() 
:
	_host(),
	_port(),
	_server_names(),
	_root(),
	_index(),
	_autoindex(true),
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

std::string	Server::getFullPath(const Request& request) const
{
	const Location* location = getMatchingLocation(request.getPath());

	/* debug */std::cout << PINK << "> SVR: req path: " << RESET << request.getPath() << std::endl;
	/* debug */std::cout << PINK << "> SVR: location found: " << RESET << (location ? "YES" : "NO") << std::endl;
	/* debug */if (location)
	/* debug */{
	/* debug */		std::cout << PINK << "> SVR: location path: " << RESET << location->_path << std::endl;
	/* debug */		std::cout << PINK << "> SVR: location root: " << RESET << location->_root << std::endl;
	/* debug */}
	/* debug */std::cout << PINK << "> SVR: server root: " << RESET << _root << std::endl;

	std::string path = request.getPath();
	std::string root = _root;

	// if location has custom root, use it and strip the location prefix
	if (location && !location->_root.empty())
	{
		/* debug */std::cout << PINK << "> SVR: using loc custom root" << RESET << std::endl;
		root = location->_root;

		// strip location from prefix path
		if (!location->_path.empty())
		{
			size_t loc_len = location->_path.length();
			if (path.compare(0, loc_len, location->_path) == 0)
			{
				path = path.substr(loc_len);
				// ensure path starts with /
				if (path.empty() || path[0] != '/')
					path = "/" + path;
			}
		}
	}
	else
		/* debug */std::cout << PINK << "> SVR: using server root, keeping full path" << RESET << std::endl;
	return root + path;
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

const Location* Server::getMatchingLocation(const std::string& requestPath) const {
	const Location* best_match = nullptr;
	size_t best_len = 0;

	/* debug */std::cout << PINK << "> SVR: matching for req: " << RESET << requestPath << "'" << std::endl;
	for (size_t i = 0; i < _locations.size(); ++i) {
		const Location& loc = _locations[i];
		// /* debug */std::cout << PINK << "> check location path: " << RESET << loc._path << "'" << std::endl;
		size_t len = loc._path.length();
		if (requestPath.compare(0, len, loc._path) == 0 
			&& (requestPath.length() == len || requestPath[len] == '/')) {
			if (len > best_len) {
				best_len = len;
				best_match = &_locations[i];
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
	this->_check.line_count = -1;
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
