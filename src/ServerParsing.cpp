#include "Webserv.hpp"

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
{
    // set default error page
    addErrorPage(404, _root + "/error.html");
} 

Server::~Server() {}

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

std::vector<Location>& Server::getLocations() {
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

const Location* Server::bestMatchingLocation(const std::string& requestPath) const {
	const Location* best_match = nullptr;
	size_t best_len = 0;

	for (size_t i = 0; i < _locations.size(); ++i) {
		const Location& loc = _locations[i];
		size_t len = loc.path.length();
		if (requestPath.compare(0, len, loc.path) == 0 
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

bool Server::isDirectory(const std::string& path) const {
    struct stat pathStat;
    if (stat(path.c_str(), &pathStat) != 0) {
        return false; // error accessing path
    }

    return S_ISDIR(pathStat.st_mode);
}

bool Server::isFile(const std::string& path) const {
    struct stat pathStat;
    if (stat(path.c_str(), &pathStat) != 0) {
        return false; // error accessing path
    }

    // Check if it's a regular file
    return S_ISREG(pathStat.st_mode);
}

