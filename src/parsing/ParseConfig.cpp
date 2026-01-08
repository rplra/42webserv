#include "Utils.hpp"
#include "Config.hpp"
#include "ConfigParse.hpp"

/*
   keywords referenced:
   global, server & location context
   https://www.alimnaqvi.com/blog/webserv
 */

/* read up to ; only */

template <typename T>
bool	Server::checkCommonDirective(std::string str, std::istringstream &iss, T &data)
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
			// /*debug*/ std::cout << str << std::endl;
			parseCommonDirective(i, iss, data);
			return (1);	//save data
		}
	}
	return (0);
}

bool	Server::handleServerDirective(std::string str, std::istringstream &iss, std::ifstream &inFile)
{
	const char *arr[] =
	{
		"listen",
		"server_name",
		"location"
	};
	std::vector<std::string> types(arr, arr + 3);
	for (std::size_t i=0; i < types.size(); i++)
	{
		if (types[i] == str)
		{
			parseServerDirective(i, iss, inFile);
			return (1);
		}
	}
	return (0);
}

void	Server::parseListen(std::istringstream &iss)
{
	std::string word, port;

	if (iss >> word)
	{
		port = trimStringHead(word, ':');
		std::istringstream(port) >> this->_port;

		this->_host = trimStringTail(word, ':');
		if (this->_host == port)
			this->_host = "0.0.0.0"; // set this if no ip specified
	}
}

template <typename T>
void	Server::parseCommonDirective(std::size_t code, std::istringstream &iss, T &data)
{
	std::string word;

	if (code != ERROR_PAGE)
	{
		iss >> word;
		// word = trimStringTail(word, ';');
	}
	switch (code)
	{
		case ROOT:
			data._root = word;
			// data.setRoot(word);
			// /*debug*/ std::cout << "root: " << data._root << std::endl;
			break;

		case INDEX:
			data._index = word;
			// data.setIndex(word);
			// /*debug*/ std::cout << data._index << std::endl;
			break;

		case AUTOINDEX_DIR:
			if (word == "on")
				data._autoindex = 1;
			else if (word == "off")
				data._autoindex = 0;
			// /*debug*/ std::cout << data._autoindex << std::endl;
			break;

		case ERROR_PAGE:
			assignMapContainer(data._error_pages, iss);
			break;

		case CLIENT_MAX_BODY_SIZE:
			std::istringstream(word) >> data._client_max_body_size;
			// /*debug*/std::cout << data._client_max_body_size << std::endl;
			break;

		default:
			break;
	}
}

void	Server::parseServerDirective(std::size_t code, std::istringstream &iss, std::ifstream &inFile)
{
	switch (code)
	{
		case LISTEN:
			parseListen(iss);
			break;

		case SERVER_NAME:
			assignVecContainer(this->_server_names, iss);
			break;

		case LOCATION:
			parseLocation(inFile, iss);

		default:
			break;
	}
}

void	Server::parseLocation(std::ifstream &inFile, std::istringstream &iss)
{
	std::string			buffer, word;
	Location			tmp;

	initLocation(tmp);
	iss >> tmp._path;
	while (std::getline(inFile, buffer))
	{
		buffer = trimStringTail(buffer, ';');
		// /*debug*/ std::cout << PINK << buffer << RESET << std::endl;

		iss.clear();
		iss.str(buffer);
		if (!(iss >> word))
			continue ;
		if (word == "}")
		{
			this->_locations.push_back(tmp);
			return ;
		}
		checkCommonDirective(word, iss, tmp);
		handleLocationDirective(word, iss, tmp);
	}
}

void	Server::parseServer(std::ifstream &inFile)
{
	std::string			buffer, word;
	std::istringstream	iss;

	while (std::getline(inFile, buffer))
	{
		buffer = trimStringTail(buffer, ';');
		// /*debug*/ std::cout << YELLOW << buffer << RESET << std::endl;
		iss.clear();
		iss.str(buffer);
		if (!(iss >> word))
			continue ;
		if (word == "}")
			break ;
		checkCommonDirective(word, iss, *this);
		handleServerDirective(word, iss, inFile);
	}
}

/*
 check if it is server or common_directives
	- if server:
		- check if is location or common_directives
		- if location:
			- check if is common_directives or location_dir
 */

void	ConfigParser::startParser(std::ifstream &inFile)
{
	/* get tokens */
	std::string				buffer, word;
	std::istringstream		iss;

	while (std::getline(inFile, buffer))
	{
		iss.clear();
		iss.str(buffer);
		if (!(iss >> word) || word == "#")
			continue ;
		if (word == "server")
		{
			Server tmp;
			// /*debug*/std::cout << PINK << word << RESET << std::endl;
			tmp.parseServer(inFile);
			this->_config.getServers().push_back(tmp);
		}
		else
			throw (std::invalid_argument(ERR_DIRECTIVEINVALID));
	}
}

void	ConfigParser::parseConfig(char **av)
{
	std::string		filename = av[1];
	std::ifstream	inFile(filename.c_str());

	try
	{
		if (!inFile.is_open())
			throw (std::invalid_argument(ERR_FILEINVALID));

		// if filename doesnt end with .conf
		size_t pos = filename.rfind('.');
		if (pos == std::string::npos || filename.substr(pos) != ".conf")
			throw (std::invalid_argument(ERR_FILEEXTINVALID));

		if (inFile.peek() == EOF)
			throw (std::invalid_argument(ERR_FILEEMPTY));
		/* scan all errors */
		this->errorCheckConfig(inFile); // throws error here

		/* else, start parsing */
		this->startParser(inFile);
		// /*debug*/ this->_config.printAllServer();

		/* close after reading */
		inFile.close();
		return ;
	}
	catch (std::exception &err)
	{
		std::cerr	<< RED << "Exception: " << err.what();
		if (this->_check.keyword.length() > 0)
			std::cerr << " \"" << this->_check.keyword << "\"";

		std::cerr	<< " [line " << this->_check.line_count << "]"
					<< RESET << std::endl;
	}
	exit(1);
}

ConfigParser::ConfigParser(Config& target) : _config(target)
{
	this->_check.line_count = -1;
}