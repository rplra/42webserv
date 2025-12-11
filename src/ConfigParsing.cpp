#include "Webserv.hpp"

/*
   keywords referenced:
   global, server & location context
   https://www.alimnaqvi.com/blog/webserv
 */
Config::Config()
:
	_servers(),
	_port_map(),
	_global_scope(NONE_GLOBAL),
	_server_scope(NONE_SERVER),
	_location_scope(NONE_LOCATION),
	_common_directive(NONE_COMMON)
{}

Config::~Config() {}

const std::vector<Server>& Config::getServers() const {
	return _servers;
}

void Config::block_validation(std::string buffer) {
	if (_global_scope == NONE_GLOBAL && trim(buffer) == "server {")
	{
		Server newServer;
		_servers.push_back(newServer);
		_global_scope = SERVER;
		// std::cout << GREEN << "Global Scope: " << _global_scope << RESET << std::endl;
	}
	else if (_global_scope == SERVER && trim(buffer) == "server {") {
		std::cerr << RED << "Error: Unexpected closing brace '}'" << RESET << std::endl;
		throw std::invalid_argument("Unexpected closing brace");
	}

	// location parsing inside server
	if (_global_scope == SERVER && _server_scope == NONE_SERVER && trim(buffer).find("location") != std::string::npos) {
		_server_scope = LOCATION;

		std::istringstream	iss(buffer);
		std::string			directive;
		std::string			path;
		std::string			opening_brace;

		iss >> directive >> path >> opening_brace;
		if (opening_brace != "{") 
			std::cerr << RED << "Error: Missing opening brace '{' for location block" << RESET << std::endl;
		
		Location newLocation;
		newLocation.path = path;
		Server& currentServer = _servers.back();
		newLocation.autoindex = false;
		if (currentServer.getAutoindex())
			newLocation.autoindex = true;
		newLocation.client_max_body_size = 0;
		currentServer.addLocation(newLocation);
	} 
	else if (_global_scope == SERVER && _server_scope == LOCATION && trim(buffer).find("location") != std::string::npos){
		std::cerr << RED << "Error: Unexpected closing brace '}'" << RESET << std::endl;
		throw std::invalid_argument("Unexpected closing brace");
	}

	if (_server_scope == LOCATION && trim(buffer) == "}") {
		_server_scope = NONE_SERVER;
	}
	else if (_server_scope == NONE_SERVER && _global_scope == SERVER && trim(buffer) == "}") {
		_global_scope = NONE_GLOBAL;
	}

	else if (trim(buffer) == "}") {
		std::cerr << RED << "Error: Unexpected closing brace '}'" << RESET << std::endl;
		throw std::invalid_argument("Unexpected closing brace");
	}
}

std::string Config::removeSemicolon(std::string str) {
	size_t pos = str.find(";");
	if (pos != std::string::npos) {
		return str.substr(0, pos);
	}
	return str;
}

void	Config::parseConfig(char **av)
{
	std::ifstream inFile(av[1]);

	try
	{
		if (!inFile.is_open())
			throw (std::invalid_argument(ERR_FILEINVALID));
		if (inFile.peek() == EOF)
			throw (std::invalid_argument(ERR_FILEEMPTY));
		
		/* else, start parsing */
		std::string buffer;
		while (std::getline(inFile, buffer))
		{
			std::istringstream	iss(buffer);
			// std::cout << buffer << std::endl;
			
			block_validation(buffer);

			Server& currentServer = _servers.back();
			if (_global_scope == SERVER && _server_scope != LOCATION)
			{
				if (buffer.find("listen") != std::string::npos)
				{
					// parse listen directive
					std::istringstream	iss(buffer);
					std::string			directive;
					std::string			value;
					
					iss >> directive >> value;
					if (value.find(":") != std::string::npos)
					{
						size_t colonPos = value.find(":");
						std::string host = value.substr(0, colonPos);
						int port = std::stoi(value.substr(colonPos + 1));
						currentServer.setHost(host);
						currentServer.setPort(port);

						// update port map
						_port_map[port].push_back(&currentServer);
					}
					else if (isdigit(value[0]))
					{
						int port = std::stoi(value);
						currentServer.setPort(port);
						_port_map[port].push_back(&currentServer);
					}
				}
				else if (buffer.find("server_name") != std::string::npos)
				{
					// parse server_name directive
					std::istringstream	iss(buffer);
					std::string			directive;
					std::string			value;
					
					iss >> directive;
					while (iss >> value)
					{
						value = removeSemicolon(value);
						currentServer.addServerName(value);
					}
				}
				else if (buffer.find("root") != std::string::npos)
				{
					// parse root directive
					std::istringstream	iss(buffer);
					std::string			directive;
					std::string			root;
					
					iss >> directive >> root;
					root = removeSemicolon(root);
					currentServer.setRoot(root);
					// std::cout << GREEN << "Set server root to: " << currentServer.getRoot() << RESET << std::endl;
				}
				else if (trim(buffer).compare(0, 9, "autoindex") == 0)
				{
					// parse autoindex directive
					std::istringstream	iss(buffer);
					std::string			directive;
					std::string			value;
					
					iss >> directive >> value;
					value = removeSemicolon(value);
					bool autoindex = (value == "on") ? true : false;
					currentServer.setAutoindex(autoindex);
					// std::cout << GREEN << "Set server autoindex to: " << (currentServer.getAutoindex() ? "on" : "off") << RESET << std::endl;
				}
				else if ((trim(buffer).compare(0, 5, "index") == 0) && _server_scope == NONE_LOCATION)
				{
					// parse index directive
					std::istringstream iss(buffer);
					std::string directive;
					std::string index;

					iss >> directive >> index;
					index = removeSemicolon(index);
					currentServer.setIndex(index);
					// std::cout << GREEN << "Set server index to: " << currentServer.getIndex() << RESET << std::endl;
				}
				else if (buffer.find("client_max_body_size") != std::string::npos)
				{
					// parse client_max_body_size directive 
					std::istringstream iss(buffer);
					std::string directive;
					std::string sizeStr;

					iss >> directive >> sizeStr;
					sizeStr = removeSemicolon(sizeStr);
					size_t size = std::stoul(sizeStr);
					currentServer.setClientMaxBodySize(size);
					// std::cout << GREEN << "Set client max body size to: " << currentServer.getClientMaxBodySize() << RESET << std::endl;
				}
				else if (buffer.find("error_page") != std::string::npos)
				{
					// parse error_page directive 
					std::istringstream iss1(buffer);
					std::istringstream iss2(buffer);
					std::string directive;
					std::string value;
					std::string path;

					iss1 >> directive;
					while (iss1 >> value)
						if (value.find(";") != std::string::npos)
							path = removeSemicolon(value);
					// std::cout << GREEN << "Parsed error page path: " << path << RESET << std::endl;
					
					iss2 >> directive;
					while (iss2 >> value && value.find(";") == std::string::npos)
					{
						int errorCode = std::stoi(value);
						currentServer.addErrorPage(errorCode, path);
						// std::cout << GREEN << "Added error page for code " << errorCode << " with path: " << path << RESET << std::endl;
					}
				}
			}
			else if (_server_scope == LOCATION)
			{
				Location& currentLocation = currentServer.getLocations().back();
				
				if (trim(buffer).compare(0, 5, "index") == 0)
				{
					// parse index directive inside location
					std::istringstream	iss(buffer);
					std::string			directive;
					std::string			index;
					
					iss >> directive >> index;
					index = removeSemicolon(index);
					currentLocation.index = index;
					// std::cout << GREEN << "Set location index to: " << currentLocation.index << RESET << std::endl;
				}
				else if (buffer.find("allowed_methods") != std::string::npos)
				{
					// parse allowed_methods directive inside location
					std::istringstream	iss(buffer);
					std::string			directive;
					std::string			method;
					
					iss >> directive;
					while (iss >> method)
					{
						method = removeSemicolon(method);
						currentLocation.allowed_methods.push_back(method);
						// std::cout << GREEN << "Added allowed method: " << method << RESET << std::endl;
					}
				}
				else if (buffer.find("return") != std::string::npos)
				{
					// parse return directive inside location
					std::istringstream	iss(buffer);
					std::string			directive;
					std::string			codeStr;
					std::string			redirectPath;
					
					iss >> directive >> codeStr >> redirectPath;
					redirectPath = removeSemicolon(redirectPath);
					int returnCode = std::stoi(codeStr);
					currentLocation.redirect[returnCode] = redirectPath;
					// std::cout << GREEN << "Added return directive: " << returnCode << " -> " << redirectPath << RESET << std::endl;
				}
				else if (buffer.find("client_max_body_size") != std::string::npos)
				{
					std::istringstream iss(buffer);
					std::string 	directive;
					std::string 	sizeStr;

					iss >> directive >> sizeStr;
					sizeStr = removeSemicolon(sizeStr);
					size_t size = std::stoul(sizeStr);
					currentLocation.client_max_body_size = size;
					// std::cout << GREEN << "Set location client max body size to: " << currentLocation.client_max_body_size << RESET << std::endl;
				}
				else if (buffer.find("root") != std::string::npos) 
				{
					std::istringstream iss(buffer);
					std::string 	   directive;
					std::string 	   root;

					iss >> directive >> root;
					root = removeSemicolon(root);
					currentLocation.root = root;
					// std::cout << GREEN << "Set location root to: " << currentLocation.root << RESET << std::endl;
				}
				else if (trim(buffer).compare(0, 9, "autoindex") == 0)
				{
					std::istringstream	iss(buffer);
					std::string			directive;
					std::string			value;
					
					iss >> directive >> value;
					value = removeSemicolon(value);
					bool autoindex = (value == "on") ? true : false;
					currentLocation.autoindex = autoindex;
					// std::cout << GREEN << "Set location autoindex to: " << (currentLocation.autoindex ? "on" : "off") << RESET << std::endl;	
				}
				else if (trim(buffer).compare(0, 10, "error_page") == 0)
				{
					// parse error_page directive inside location
					std::istringstream iss1(buffer);
					std::istringstream iss2(buffer);
					std::string directive;
					std::string value;
					std::string path;

					iss1 >> directive;
					while (iss1 >> value)
						if (value.find(";") != std::string::npos)
							path = removeSemicolon(value);
					// std::cout << GREEN << "Parsed error page path: " << path << RESET << std::endl;
					
					iss2 >> directive;
					while (iss2 >> value && value.find(";") == std::string::npos)
					{
						int errorCode = std::stoi(value);
						currentLocation.error_pages[errorCode] = path;
						// std::cout << GREEN << "Added error page for code " << errorCode << " with path: " << path << RESET << std::endl;
					}
				}
				else if (trim(buffer).compare(0, 3, "cgi") == 0)
				{
					// parse cgi directive inside location
					std::istringstream	iss(buffer);
					std::string			directive;
					std::string			extension;
					std::string			cgiPath;

					iss >> directive >> extension >> cgiPath;
					cgiPath = removeSemicolon(cgiPath);
					currentLocation.cgi[extension] = cgiPath;
					std::cout << GREEN << "Added CGI: " << extension << " -> " << cgiPath << RESET << std::endl;
				}
			}
		}	
		// std::cout << GREEN << getServers().size() << " server(s) parsed successfully." << RESET << std::endl;
		/* reset after reading */
		inFile.close();
	}
	catch (std::exception &err)
	{
		std::cout	<< RED
					<< "Exception: " << err.what()
					<< RESET << std::endl;
		exit(1);
	}
}