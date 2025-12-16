#include "Utils.hpp"
#include "Config.hpp"
#include "ConfigParse.hpp"
#include <limits.h>

void	ConfigParser::checkValidTypeRoot(std::istringstream &iss, errCheckGroup &data)
{
	struct stat			sb;
	std::string			word;
	std::istringstream	tmp_iss(iss.str());

	tmp_iss.seekg(iss.tellg());
	tmp_iss >> word;
	if (stat(word.c_str(), &sb) == 0 &&	S_ISDIR(sb.st_mode))
	{
		data.root = word;
		return ;
	}
	this->_check.keyword = word;
	throw (std::invalid_argument(ERR_INVALIDPATH));
}

void	ConfigParser::checkValidTypeIndex(std::istringstream &iss, errCheckGroup &data)
{
	struct stat			sb;
	std::string			word, path;
	std::istringstream	tmp_iss(iss.str());

	tmp_iss.seekg(iss.tellg());
	tmp_iss >> word;
	path = data.root + "/" + word;

	if (stat(path.c_str(), &sb) == 0 &&	S_ISREG(sb.st_mode))
		return ;

	this->_check.keyword = word;
	throw (std::invalid_argument(ERR_FILENOTFOUND));
}

void	ConfigParser::checkValidTypeErrPage(std::istringstream &iss, errCheckGroup &data)
{
	struct stat			sb;
	std::string			path, word;
	int					err_code;
	std::istringstream	tmp_iss(iss.str());

	tmp_iss.seekg(iss.tellg());
	tmp_iss >> err_code;
	tmp_iss >> word;
	path = data.root + "/" + word;

	if (err_code < 100 || err_code > 599)
	{
		std::ostringstream oss;
		oss << err_code;
		this->_check.keyword = oss.str();
		throw (std::invalid_argument(ERR_CODEINVALID));
	}
	else if (stat(path.c_str(), &sb) != 0 || !S_ISREG(sb.st_mode))
	{
		this->_check.keyword = word;
		throw (std::invalid_argument(ERR_FILENOTFOUND));
	}
}

void	ConfigParser::checkValidTypeMaxBodySize(std::istringstream &iss)
{
	std::string			word;
	int					num;
	std::istringstream	tmp_iss(iss.str());
	tmp_iss.seekg(iss.tellg());

	tmp_iss >> num;
	if (num > 0)
		return ;
	
	std::ostringstream	oss;
	oss << num;
	this->_check.keyword = oss.str();
	throw (std::invalid_argument(ERR_BODYSIZEINVALID));
}

void	ConfigParser::checkValidTypeAutoindex(std::istringstream &iss)
{
	std::string			word;
	std::istringstream	tmp_iss(iss.str());
	tmp_iss.seekg(iss.tellg());

	const char *types[] =
	{
		"on",
		"off",
		NULL
	};

	while (tmp_iss >> word)
	{
		word = trimStringTail(word, ';');
		// /*debug*/ std::cout << PINK << "checkAllowedAutoindex: " << word << std::endl;
		checkMatch(types, word, ERR_TYPEUNSUPPORTED);
	}
}

void	ConfigParser::checkValidTypeCommon(size_t code, std::istringstream &iss, errCheckGroup &data)
{
	switch (code)
	{
		case ROOT:
			checkValidTypeRoot(iss, data);
			break ;
		case INDEX:
			// checkValidTypeIndex(iss, data);
			break ;
		case ERROR_PAGE:
			checkValidTypeErrPage(iss, data);
			break ;
		case AUTOINDEX_DIR:
			checkValidTypeAutoindex(iss);
			break ;
		case CLIENT_MAX_BODY_SIZE:
			checkValidTypeMaxBodySize(iss);
	}
}