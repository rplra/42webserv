#include "ConfigParser.hpp"

/*
   keywords referenced:
   global, server & location context
   https://www.alimnaqvi.com/blog/webserv
 */

ConfigParser::ConfigParser(Config& config)
:	_config(config),
	_global_scope(NONE_GLOBAL),
	_server_scope(NONE_SERVER)
	// _location_scope(NONE_LOCATION),
	// _common_directive(NONE_COMMON)
{}

/*********************************  TO UPDATE FOR INTEGRATION *********************************/
// void	ConfigParser::parseConfig(std::string &filename)
// {
// 	std::ifstream inFile(filename);

// check for extension format - .conf (need to check?)

// 	try
// 	{
// 		if (!inFile.is_open())
// 			throw (std::invalid_argument(ERR_FILEINVALID));
// 		if (inFile.peek() == EOF)
// 			throw (std::invalid_argument(ERR_FILEEMPTY));
// 		/* scan all errors */

// 		/* else, start parsing */
// 		this->startParser(inFile);
// 		/*debug*/ this->printAllServer();

// 		/* reset after reading */
// 		inFile.close();
// 		return ;
// 	}
// 	catch (std::exception &err)
// 	{
// 		std::cout	<< RED
// 					<< "Exception: " << err.what()
// 					<< RESET << std::endl;
// 	}
// 	exit(1);
// }

// void	ConfigParser::parseServer(std::ifstream &inFile)
// {
// 	std::string			buffer, word;
// 	std::istringstream	iss;
// 	std::streampos		pos = inFile.tellg();

// 	while (std::getline(inFile, buffer))
// 	{
// 		// /*debug*/ std::cout << YELLOW << buffer << RESET << std::endl;
// 		iss.clear();
// 		iss.str(buffer);
// 		if (!(iss >> word))
// 			continue ;
// 		if (word == "server")
// 		{
// 			inFile.seekg(pos);	//rewind back
// 			break;
// 		}
// 		pos = inFile.tellg();
// 		checkCommonDirective(word, iss, *this);
// 		checkServerDirective(word, iss, inFile);
// 		// if (checkCommonDirective(word, iss, *this))
// 		// 	std::cout << RED << word << RESET << std::endl;
// 		// else if (checkServerDirective(word, iss, inFile))
// 		// 	std::cout << BLUE << word << RESET << std::endl;
// 		// while (iss >> word)
// 			// std::cout << word << std::endl;
// 	}
// }

// void	ConfigParser::assignMapContainer(std::map<int, std::string> &data, std::istringstream &iss)
// {
// 	std::string	word;
// 	int			err_code;

// 	iss >> err_code;
// 	iss >> word;
// 	word = trimStringTail(word, ';');
// 	data[err_code] = word;
// }

// void	ConfigParser::assignVecContainer(std::vector<std::string> &data, std::istringstream &iss)
// {
// 	std::string word;

// 	while (iss >> word)
// 	{
// 		word = trimStringTail(word, ';');
// 		data.push_back(word);
// 	}
// }

// void	ConfigParser::startParser(std::ifstream &inFile)
// {
// 	/* error checks */

// 	/* get tokens */
// 	std::string				buffer, word;
// 	std::istringstream		iss;

// 	while (std::getline(inFile, buffer))
// 	{
// 		iss.clear();
// 		iss.str(buffer);
// 		if (!(iss >> word))
// 			continue ;
// 		if (word == "server")
// 		{
// 			Server tmp;
// 			/*debug*/std::cout << PINK << word << RESET << std::endl;
// 			tmp.parseServer(inFile);
// 			this->_servers.push_back(tmp);
// 		}
// 		// else
// 			// /*debug*/std::cout << word << std::endl;
// 	}
// }


// template <typename T>
// bool	ConfigParser::checkCommonDirective(std::string str, std::istringstream &iss, T &data)
// {
// 	const char *arr[] =
// 	{
// 		"root",
// 		"index",
// 		"autoindex",
// 		"error_page",
// 		"client_max_body_size"
// 	};
// 	std::vector<std::string>	types(arr, arr + 5);
// 	for (std::size_t i=0; i < types.size(); i++)
// 	{
// 		if (types[i] == str)
// 		{
// 			getCommonDirective(i, iss, data);
// 			return (1);	//save data
// 		}
// 	}
// 	return (0);
// }

// bool	ConfigParser::checkServerDirective(std::string str, std::istringstream &iss, std::ifstream &inFile)
// {
// 	const char *arr[] =
// 	{
// 		"listen",
// 		"server_name",
// 		"location"
// 	};
// 	std::vector<std::string> types(arr, arr + 3);
// 	for (std::size_t i=0; i < types.size(); i++)
// 	{
// 		if (types[i] == str)
// 		{
// 			getServerDirective(i, iss, inFile);
// 			return (1);
// 		}
// 	}
// 	return (0);
// }

// bool	ConfigParser::checkLocationDirective(std::string str, std::istringstream &iss, Location& data)
// {
// 	const char *arr[] =
// 	{
// 		"cgi_handler",
// 		"allowed_methods",
// 		"upload_store",
// 		"return"
// 	};
// 	std::vector<std::string> types(arr, arr + 4);
// 	for (std::size_t i=0; i < types.size(); i++)
// 	{
// 		if (types[i] == str)
// 		{
// 			getLocationDirective(i, iss, data);
// 			return (1);
// 		}
// 	}
// 	return (0);
// }

// void	ConfigParser::getListen(std::istringstream &iss)
// {
// 	std::string word, port;

// 	if (iss >> word)
// 	{
// 		port = trimStringHead(word, ':');
// 		port = trimStringTail(port, ';');
// 		std::istringstream(port) >> this->_port;

// 		this->_host = trimStringTail(word, ':');
// 	}
// }

// template <typename T>
// void	ConfigParser::getCommonDirective(std::size_t code, std::istringstream &iss, T &data)
// {
// 	std::string word;

// 	if (code != ERROR_PAGE)
// 	{
// 		iss >> word;
// 		word = trimStringTail(word, ';');
// 	}
// 	switch (code)
// 	{
// 		case ROOT:
// 			data._root = word;
// 			// /*debug*/ std::cout << data._root << std::endl;
// 			break;

// 		case INDEX:
// 			data._index = word;
// 			// /*debug*/ std::cout << data._index << std::endl;
// 			break;

// 		case AUTOINDEX:
// 			if (word == "on")
// 				data._autoindex = 1;
// 			else if (word == "off")
// 				data._autoindex = 0;
// 			// /*debug*/ std::cout << data._autoindex << std::endl;
// 			break;

// 		case ERROR_PAGE:
// 			assignMapContainer(data._error_pages, iss);
// 			break;

// 		case CLIENT_MAX_BODY_SIZE:
// 			std::istringstream(word) >> data._client_max_body_size;
// 			// /*debug*/std::cout << data._client_max_body_size << std::endl;
// 			break;

// 		default:
// 			break;
// 	}
// }

// void	ConfigParser::getServerDirective(std::size_t code, std::istringstream &iss, std::ifstream &inFile)
// {
// 	switch (code)
// 	{
// 		case LISTEN:
// 			getListen(iss);
// 			break;

// 		case SERVER_NAME:
// 			assignVecContainer(this->_server_names, iss);
// 			break;

// 		case LOCATION:
// 			parseLocation(inFile, iss);

// 		default:
// 			break;
// 	}
// }

// void	ConfigParser::getLocationDirective(std::size_t code, std::istringstream &iss, Location &data)
// {
// 	std::string word;

// 	switch (code)
// 	{
// 		case CGI_HANDLER:
// 			assignMapContainer(data._cgi, iss);
// 			break;

// 		case ALLOWED_METHODS:
// 			assignVecContainer(data._allowed_methods, iss);
// 			break;

// 		case UPLOAD_STORE:
// 			iss >> word;
// 			data._upload_path = trimStringTail(word, ';');
// 			break;

// 		case RETURN:
// 			assignMapContainer(data._error_pages, iss);
// 			break;

// 		default:
// 			break;
// 	}
// }

// void	ConfigParser::parseLocation(std::ifstream &inFile, std::istringstream &iss)
// {
// 	std::string			buffer, word;
// 	Location			tmp;

// 	initLocation(tmp);
// 	iss >> tmp._path;
// 	// /*debug*/std::cout << "path: " << tmp._path << std::endl;
// 	while (std::getline(inFile, buffer))
// 	{
// 		iss.clear();
// 		iss.str(buffer);
// 		if (!(iss >> word))
// 			continue ;
// 		if (word == "}")
// 		{
// 			this->_locations.push_back(tmp);
// 			return ;
// 		}
// 		checkCommonDirective(word, iss, tmp);
// 		checkLocationDirective(word, iss, tmp);
// 		// if (checkCommonDirective(word, iss, tmp))
// 		// 	std::cout << RED << word << RESET << std::endl;
// 		// else if (checkLocationDirective(word, iss, tmp))
// 		// 	std::cout << PINK << word << RESET << std::endl;
// 	}
// }

// void	ConfigParser::printErrorPage()
// {
// 	for (std::map<int, std::string>::const_iterator it = this->_error_pages.begin(); 
// 	it != this->_error_pages.end(); ++it)
// 		std::cout << it->first << " = " << it->second << std::endl;
// }

// /* debug functions */
// void	ConfigParser::printServer()
// {
// 	std::cout << "host          : " << this->_host << std::endl;
// 	std::cout << "port          : " << this->_port << std::endl;
// 	std::cout << "server_name   : " << std::endl;
// 	printVectorContainer(this->_server_names);
// 	std::cout << "root          : " << this->_root << std::endl;
// 	std::cout << "index         : " << this->_index << std::endl;
// 	std::cout << "autoindex     : " << this->_autoindex << std::endl;
// 	std::cout << "client_body   : " << this->_client_max_body_size << std::endl;
// 	std::cout << "error_pages   : " << std::endl;
// 	printMapContainer(this->_error_pages);
// 	std::cout << "locations     : " << std::endl;
// 	printLocations(this->_locations);
// }

// void	ConfigParser::printAllServer()
// {
// 	std::vector<Server>::iterator it	= this->_servers.begin();
// 	std::vector<Server>::iterator ite	= this->_servers.end();

// 	while (it != ite)
// 	{
// 		std::cout	<< YELLOW << ">> SERVER ---------------------------------------"
// 					<< RESET << std::endl;
// 		it->printServer();
// 		it++;
// 	}
// }

// void	ConfigParser::initLocation(Location &obj)
// {
// 	obj._autoindex = 0;
// 	obj._client_max_body_size = this->_client_max_body_size;
// }



/*********************************  TO REPLACE *********************************/
void	ConfigParser::parseConfig(std::string &filename)
{
	std::ifstream inFile(filename);

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

			Server& currentServer = _config.getServers().back();
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
						_config.getServerOnPort(port).push_back(&currentServer);
					}
					else if (isdigit(value[0]))
					{
						int port = std::stoi(value);
						currentServer.setPort(port);
						_config.getServerOnPort(port).push_back(&currentServer);
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
					currentLocation._index = index;
					// std::cout << GREEN << "Set location index to: " << currentLocation._index << RESET << std::endl;
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
						currentLocation._allowed_methods.push_back(method);
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
					currentLocation._redirect[returnCode] = redirectPath;
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
					currentLocation._client_max_body_size = size;
					// std::cout << GREEN << "Set location client max body size to: " << currentLocation.client_max_body_size << RESET << std::endl;
				}
				else if (buffer.find("root") != std::string::npos) 
				{
					std::istringstream iss(buffer);
					std::string 	   directive;
					std::string 	   root;

					iss >> directive >> root;
					root = removeSemicolon(root);
					currentLocation._root = root;
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
					currentLocation._autoindex = autoindex;
					// std::cout << GREEN << "Set location autoindex to: " << (currentLocation._autoindex ? "on" : "off") << RESET << std::endl;	
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
						currentLocation._error_pages[errorCode] = path;
						std::cout << GREEN << "Added error page for code " << errorCode << " with path: " << path << RESET << std::endl;
					}
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

void ConfigParser::block_validation(std::string buffer)
{
	if (_global_scope == NONE_GLOBAL && trim(buffer) == "server {")
	{
		Server newServer;
		_config.getServers().push_back(newServer);
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
		newLocation._path = path;

		// FIX : only override with server if empty
		newLocation._root = "";
		newLocation._autoindex = false;
		newLocation._client_max_body_size = 0;
		Server& currentServer = _config.getServers().back();
		// newLocation._root = currentServer.getRoot();
		// newLocation._autoindex = currentServer.getAutoindex();
		// newLocation._client_max_body_size = currentServer.getClientMaxBodySize();

		// newLocation._autoindex = false;
		// if (currentServer.getAutoindex())
		// 	newLocation._autoindex = true;
		// newLocation._client_max_body_size = 0;
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

std::string ConfigParser::removeSemicolon(std::string str) {
	size_t pos = str.find(";");
	if (pos != std::string::npos) {
		return str.substr(0, pos);
	}
	return str;
}
/*********************************  TO REPLACE *********************************/
