#include "Utils.hpp"
#include "Config.hpp"
#include "ConfigParse.hpp"

/*
   keywords referenced:
   global, server & location context
   https://www.alimnaqvi.com/blog/webserv
 */

/* read up to ; only */

Server::Server()
{

}

Server::~Server()
{

}

template <typename T>
bool	Server::isCommonDirective(std::string str, std::istringstream &iss, T &data)
{
	const char *arr[] =
	{
		"root",
		"index",
		"autoindex",
		"error_page",
		"client_max_body_size"
	};
	std::vector<std::string>	types(arr, arr + 5);
	for (std::size_t i=0; i < types.size(); i++)
	{
		if (types[i] == str)
		{
			getCommonDirective(i, iss, data);
			return (1);	//save data
		}
	}
	return (0);
}

bool	Server::isServerDirective(std::string str, std::istringstream &iss)
{
	const char *arr[] =
	{
		"listen",
		"server_name"
		// "location",
	};
	std::vector<std::string> types(arr, arr + 2);
	for (std::size_t i=0; i < types.size(); i++)
	{
		if (types[i] == str)
		{
			getServerDirective(i, iss);
			return (1);
		}
	}
	return (0);
}

bool	Server::isLocationDirective(std::string str, std::istringstream &iss, Location& data)
{
	const char *arr[] =
	{
		"cgi_handler",
		"allowed_methods",
		"upload_store",
		"return"
	};
	std::vector<std::string> types(arr, arr + 4);
	for (std::size_t i=0; i < types.size(); i++)
	{
		if (types[i] == str)
		{
			getLocationDirective(i, iss, data);
			return (1);
		}
	}
	return (0);
}

template <typename T>
void	Server::getCommonDirective(std::size_t code, std::istringstream &iss, T &data)
{
	std::string word;

	if (code != ERROR_PAGE)
	{
		iss >> word;
		word = trimSemiColon(word);
	}
	switch (code)
	{
		case ROOT:
			data._root = word;
			/*debug*/ std::cout << data._root << std::endl;
			break;

		case INDEX:
			data._index = word;
			/*debug*/ std::cout << data._index << std::endl;
			break;

		case AUTOINDEX:
			if (word == "on")
				data._autoindex = 1;
			else if (word == "off")
				data._autoindex = 0;
			/*debug*/ std::cout << data._autoindex << std::endl;
			break;

		case ERROR_PAGE:
			assignMapContainer(data._error_pages, iss);
			// /*debug*/ data.printErrorPage();
			break;

		case CLIENT_MAX_BODY_SIZE:
			std::istringstream(word) >> data._client_max_body_size;
			/*debug*/std::cout << data._client_max_body_size << std::endl;
			break;

		default:
			break;
	}
}

void	Server::getServerDirective(std::size_t code, std::istringstream &iss)
{
	// std::string word;
	// iss >> word;
	// word = trimSemiColon(word);

	switch (code)
	{
		case LISTEN:
			// std::istringstream(word) >> this->_port;
			break;
		case SERVER_NAME:
			assignVecContainer(this->_server_names, iss);
			// this->_server_names.push_back(word);
			// while (iss >> word)
			// 	this->_server_names.push_back(word);
			break;
		default:
			break;
	}
}

void	Server::getLocationDirective(std::size_t code, std::istringstream &iss, Location &data)
{
	std::string word;

	/*debug*/ std::cout << "loc: " << word << std::endl;
	switch (code)
	{
		case CGI_HANDLER:
			assignMapContainer(data._cgi, iss);
			break;

		case ALLOWED_METHODS:
			assignVecContainer(data._allowed_methods, iss);
			break;

		case UPLOAD_STORE:
			iss >> word;
			data._upload_path = trimSemiColon(word);
			break;

		case RETURN:
			assignMapContainer(data._error_pages, iss);
			break;

		default:
			break;
	}
}

void	Server::parseLocation(std::ifstream &inFile, std::istringstream &iss)
{
	std::string			buffer, word;
	Location			tmp;

	iss >> tmp._path;
	/*debug*/std::cout << "path: " << tmp._path << std::endl;
	while (std::getline(inFile, buffer))
	{
		iss.clear();
		iss.str(buffer);
		// /*debug*/std::cout << word << std::endl;
		if (!(iss >> word))
			continue ;
		if (word == "}")
		{
			this->_locations.push_back(tmp);
			// std::cout << "printLocations: " << std::endl;
			// printLocations(this->_locations);
			return ;
		}
		if (isCommonDirective(word, iss, tmp))
			std::cout << RED << word << RESET << std::endl;
		else if (isLocationDirective(word, iss, tmp))
			std::cout << PINK << word << RESET << std::endl;
		else if (word == "return")
			std::cout << "is return: " << word << std::endl;
	}
}

void	Server::parseServer(std::ifstream &inFile)
{
	std::string			buffer, word;
	std::istringstream	iss;
	std::streampos		pos = inFile.tellg();

	while (std::getline(inFile, buffer))
	{
		/*debug*/ std::cout << YELLOW << buffer << RESET << std::endl;
		iss.clear();
		iss.str(buffer);
		if (!(iss >> word))
			continue ;
		if (word == "server")
		{
			inFile.seekg(pos);	//rewind back
			break;
		}
		pos = inFile.tellg();
		if (isCommonDirective(word, iss, *this))
		{
			// 	getCommonDirective();
			std::cout << RED << word << RESET << std::endl;
		}
		else if (isServerDirective(word, iss))
		{
			// 	getServerDirective();
			std::cout << CYAN << word << RESET << std::endl;
		}
		if (word == "location")
			parseLocation(inFile, iss);
		// while (iss >> word)
			// std::cout << word << std::endl;
	}
}

/*
 check if it is server or common_directives
	- if server:
		- check if is location or common_directives
		- if location:
			- check if is common_directives or location_dir
 */

void	Config::startParser(std::ifstream &inFile)
{
	(void) inFile;
	/* error checks */

	/* get tokens */
	std::string				buffer, word;
	std::istringstream		iss;

	while (std::getline(inFile, buffer))
	{
		iss.clear();
		iss.str(buffer);
		if (!(iss >> word))
			continue ;
		if (word == "server")
		{
			Server tmp;
			std::cout << PINK << word << RESET << std::endl;
			tmp.parseServer(inFile);
			this->_servers.push_back(tmp);
			// /*debug*/tmp.printServer();
		}
		else
			std::cout << word << std::endl;
	}
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

		/* scan all errors */
		/* else, start parsing */
		this->startParser(inFile);
		/*debug*/ this->printAllServer();
		/* reset after reading */
		inFile.close();
		return ;
	}
	catch (std::exception &err)
	{
		std::cout	<< RED
					<< "Exception: " << err.what()
					<< RESET << std::endl;
	}
	exit(1);
}