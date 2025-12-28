#include "Utils.hpp"
#include "Macros.hpp"
#include "ConfigParse.hpp"

/*
   checks if there is exact match of host + port + server_names
 * ******************************************************************* */

void	ConfigParser::errorParseListen(std::istringstream &iss, errCheckPortName &tmp)
{
	std::string					word, port, host;
	std::vector<std::string>	content;
	std::istringstream			tmp_iss(iss.str());

	tmp_iss.seekg(iss.tellg());
	if (tmp_iss >> word)
	{
		port = trimStringHead(word, ':');
		host = trimStringTail(word, ':');
		if (host == port)
			host = "0.0.0.0"; // set this if no ip specified
		tmp.host = host;
		tmp.port = port;
		if (hasDuplicateHostPort(tmp))
			throw (std::invalid_argument(ERR_DUPLICATEENDPOINT));
	}
}

void	ConfigParser::errorParseServerName(std::istringstream &iss, errCheckPortName &tmp)
{
	assignVecContainer(tmp.server_names, iss);
	if (hasDuplicateHostPort(tmp))
		throw (std::invalid_argument(ERR_DUPLICATEENDPOINT));
}

bool	ConfigParser::checkDuplicateServerName(std::vector<std::string> &master, std::vector<std::string> &to_find)
{
	std::vector<std::string>::iterator it = to_find.begin();
	std::vector<std::string>::iterator ite = to_find.end();

	while (it != ite)
	{
		std::vector<std::string>::iterator it_find = std::find(master.begin(), master.end(), *it);
		if (it_find != master.end()) // if found
			return (1);
		/*debug*/std::cout << "checkDuplicateServerName: " << *it << std::endl;
		it++;
	}
	return (0);
}

bool	ConfigParser::hasDuplicateHostPort(errCheckPortName &tmp)
{
	std::vector<errCheckPortName>::iterator it = this->_check.portNameMap.begin();
	std::vector<errCheckPortName>::iterator ite = this->_check.portNameMap.end();

	if (tmp.host.empty() || tmp.port.empty())
		return (0);
	while (it != ite)
	{
		if (it->host == tmp.host && it->port == tmp.port)
			if (checkDuplicateServerName(it->server_names, tmp.server_names)) // checks tmp_serverName & _check.server_names match
				return (1);
		it++;
	}
	return (0);
}

void	ConfigParser::checkDuplicateEndpoint(size_t code, std::istringstream &iss, errCheckPortName &tmp)
{
	switch (code)
	{
		case LISTEN:
			errorParseListen(iss, tmp); break ;
		case SERVER_NAME:
			errorParseServerName(iss, tmp); break ;
	}
}
