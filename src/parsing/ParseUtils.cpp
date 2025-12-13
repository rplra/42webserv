#include "Utils.hpp"
#include "Config.hpp"
#include "ConfigParse.hpp"

/* debug functions */
void	Server::printErrorPage()
{
	for (std::map<int, std::string>::const_iterator it = this->_error_pages.begin(); 
	it != this->_error_pages.end(); ++it)
		std::cout << it->first << " = " << it->second << std::endl;
}

/* debug functions */
void	Server::printServer()
{
	std::cout << "host          : " << this->_host << std::endl;
	std::cout << "port          : " << this->_port << std::endl;
	std::cout << "server_name   : " << std::endl;
	printVectorContainer(this->_server_names);
	std::cout << "root          : " << this->_root << std::endl;
	std::cout << "index         : " << this->_index << std::endl;
	std::cout << "autoindex     : " << this->_autoindex << std::endl;
	std::cout << "client_body   : " << this->_client_max_body_size << std::endl;
	std::cout << "error_pages   : " << std::endl;
	printMapContainer(this->_error_pages);
	std::cout << "locations     : " << std::endl;
	printLocations(this->_locations);
}

void	Config::printAllServer()
{
	std::vector<Server>::iterator it	= this->_servers.begin();
	std::vector<Server>::iterator ite	= this->_servers.end();

	while (it != ite)
	{
		std::cout	<< YELLOW << ">> SERVER ---------------------------------------"
					<< RESET << std::endl;
		it->printServer();
		it++;
	}
}

void	Server::assignMapContainer(std::map<int, std::string> &data, std::istringstream &iss)
{
	std::string	word;
	int			err_code;

	iss >> err_code;
	iss >> word;
	// word = trimStringTail(word, ';');
	data[err_code] = word;
}

void	Server::assignCgiContainer(std::map<int, std::string> &data, std::istringstream &iss)
{
	std::string	word;
	size_t		i = 0;
	const char *types[] =
	{
		".py",
		".cpp",
		".js",
		".php",
		NULL
	};

	iss >> word;
	for (i=0;  types[i];  i++)
	{
		if (types[i] == word)
		{
			iss >> word;
			data[i] = word;
			return ;
		}
	}
}

void	Server::assignVecContainer(std::vector<std::string> &data, std::istringstream &iss)
{
	std::string word;

	while (iss >> word)
	{
		// word = trimStringTail(word, ';');
		data.push_back(word);
	}
}

void	Server::initLocation(Location &obj)
{
	obj._autoindex = 0;
	obj._client_max_body_size = this->_client_max_body_size;
}
