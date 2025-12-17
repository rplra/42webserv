#include "Utils.hpp"
#include "Config.hpp"
#include "ConfigParse.hpp"

/* checks argcount for location /path */
void	ConfigParser::errorLocationBase(std::istringstream &iss)
{
	std::string word;
	int			count = 0;
	while (iss >> word)
	{
		// /*debug*/ std::cout << "errorCheckLocation " << word << std::endl;
		if (word == "{" && checkDuplicate(word, this->_check.loc.brace, ERR_UNEXPECTSIGN) && noMoreBrace(iss))
			break ;
		count++;
	}
	// /*debug*/ std::cout << ", loc_arg_count: " << count << std::endl;
	if (count != 1)
		throw (std::invalid_argument(ERR_ARGCOUNTINVALID));
}

/* returns 1 if match found */
bool	ConfigParser::checkMatch(const char* types[], std::string word, const std::string err_message)
{
	for (size_t i=0;  types[i];  i++)
	{
		if (types[i] == word)
			return (1) ;
	}
	this->_check.keyword = word;
	throw (std::invalid_argument(err_message));
}

void	ConfigParser::checkValidTypeAlias(std::istringstream &iss)
{
	struct stat			sb;
	std::string			word;
	std::istringstream	tmp_iss(iss.str());
	tmp_iss.seekg(iss.tellg());

	tmp_iss >> word;
	if (stat(word.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		return ;
	this->_check.keyword = word;
	throw (std::invalid_argument(ERR_INVALIDPATH));
}

void	ConfigParser::checkValidTypeUpload(std::istringstream &iss)
{
	struct stat			sb;
	std::string			word, path;
	std::istringstream	tmp_iss(iss.str());
	tmp_iss.seekg(iss.tellg());

	tmp_iss >> word;
	path = this->_check.loc.root + "/" + word;
	if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		return ;
	else if (stat(word.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		return ;

	this->_check.keyword = word;
	throw (std::invalid_argument(ERR_INVALIDPATH));
}

/* check if input is valid for allowed_methods */
void	ConfigParser::checkValidTypeAllowed(std::istringstream &iss)
{
	std::string			word;
	std::istringstream	tmp_iss(iss.str());
	tmp_iss.seekg(iss.tellg());

	const char *types[] =
	{
		"GET",
		"POST",
		"DELETE",
		NULL
	};
	while (tmp_iss >> word)
	{
		word = trimStringTail(word, ';');
		if (checkMatch(types, word, ERR_TYPEUNSUPPORTED))
			checkDuplicate(word, this->_check.loc.dup, ERR_DUPLICATE);
	}
}

void	ConfigParser::checkValidTypeCgi(std::istringstream &iss)
{
	std::string word;
	struct stat	sb;
	std::istringstream	tmp_iss(iss.str());
	tmp_iss.seekg(iss.tellg());

	const char *types[] =
	{
		".py",
		".cpp",
		".js",
		".php",
		NULL
	};
	tmp_iss >> word;
	for (size_t i=0;  types[i];  i++)
	{
		if (types[i] == word)
		{
			tmp_iss >> word;
			word = trimStringTail(word, ';');
			if (stat(word.c_str(), &sb) == 0 && S_ISREG(sb.st_mode) \
				&& access(word.c_str(), X_OK) == 0)
				return ;
			else
			{
				this->_check.keyword = word;
				throw (std::invalid_argument(ERR_INVALIDPATH));
			}
		}
	}
	this->_check.keyword = word;
	throw (std::invalid_argument(ERR_CGIUNSUPPORTED));
}

void	ConfigParser::checkValidTypeLoc(size_t code, std::istringstream &iss)
{
	switch (code)
	{
		case CGI_HANDLER:
			checkValidTypeCgi(iss);
			break ;
		case ALIAS:
			checkValidTypeAlias(iss);
			break ;
		case ALLOWED_METHODS:
			checkValidTypeAllowed(iss);
			break ;
		case UPLOAD_STORE:
			checkValidTypeUpload(iss);
			break ;
	}
}

void	ConfigParser::checkLocationArgCount(size_t code, std::istringstream &iss)
{
	std::string word;
	int			count = countArgs(iss);

	switch (code)
	{
		case ALLOWED_METHODS:
		if (count > 0 && count <= 3)
			return ;
		break;

        case RETURN:
		case CGI_HANDLER:
		if (count == 2)
			return;
		break;

		case ALIAS:
		case UPLOAD_STORE:
		if (count == 1)
			return ;
	}
	throw (std::invalid_argument(ERR_ARGCOUNTINVALID));
}

/* return (0) == no error */
bool	ConfigParser::errorLocationDirective(std::string str, std::istringstream &iss)
{
	const char *types[] =
	{
		"cgi_handler",
		"allowed_methods",
		"upload_store",
		"return",
		"alias",
		NULL
	};
	// std::vector<std::string> types(arr, arr + 4);
	for (std::size_t i=0; types[i]; i++)
	{
		if (types[i] == str)
		{
			if (i == 4)
				checkAliasRootConflict(str, this->_check.loc.dup);
			if (i == 0)
				checkDuplicateCgi(str, iss, this->_check.loc.cgi);
			else // alias need check dup as well
				checkDuplicate(str, this->_check.loc.dup, ERR_DUPLICATE);
			
			checkLocationArgCount(i, iss);
			checkValidTypeLoc(i, iss);
			return (0);
		}
	}
	/*debug*/ std::cout << PINK << "errorLocationDirective invalid: " << str << RESET << std::endl;
    return (1);
}

void	ConfigParser::errorCheckLocation(std::istringstream &iss, std::ifstream &inFile)
{
	std::string buffer, word;

	this->_check.loc.dup.clear();
	this->_check.loc.cgi.clear();
	this->_check.loc.brace.clear();
	this->_check.loc.root = this->_check.serv.root;
	this->_check.loc.b_openBrace = 0;
	this->_check.loc.b_closeBrace = 0;

	/* checks location base arg_count */
	errorLocationBase(iss);

	/* checks arg_count in location scope{} */
	while (std::getline(inFile, buffer))
	{
		this->_check.line_count++;
		if (checkTrimSemicolon(buffer))
			buffer = trimStringTail(buffer, ';');

		// /*debug*/ std::cout << GREEN << buffer << RESET << std::endl;

		iss.clear();
		iss.str(buffer);
		if (!(iss >> word) || ignoreKeyword(word, iss, this->_check.loc))
			continue ;
		this->_check.keyword = word;

		/* updates braces && throw if no appropriate open/close brace */
		if (this->_check.loc.b_openBrace == 0)
			this->_check.loc.b_openBrace = checkBraces("{", this->_check.loc.brace);
		if (word == "}" && noMoreBrace(iss))
			return ;
		if (errorCommonDirective(word, iss, this->_check.loc) && errorLocationDirective(word, iss))
        	throw (std::invalid_argument(ERR_DIRECTIVEINVALID));
	}
	throw std::invalid_argument(ERR_ARGCOUNTINVALID);
}