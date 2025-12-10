#include "Utils.hpp"
#include "Config.hpp"
#include "ConfigParse.hpp"

/* checks argcount for location /path */
void	Config::errorLocationBase(std::istringstream &iss)
{
	std::string word;
	int			count = 0;
	while (iss >> word)
	{
		std::cout << "errorCheckLocation " << word << std::endl;
		if (word == "{" && addCheckBrace(word, this->_check.loc.brace) && noMoreBrace(iss))
			break ;
		count++;
	}
	/*debug*/ std::cout << ", loc_arg_count: " << count << std::endl;
	if (count != 1)
		throw (std::invalid_argument(ERR_ARGCOUNTINVALID));
}

void	Config::checkLocationArgCount(size_t code, std::istringstream &iss)
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

		case UPLOAD_STORE:
		if (count == 1)
			return ;
	}
	throw (std::invalid_argument(ERR_ARGCOUNTINVALID));
}

/* return (0) == no error */
bool	Config::errorLocationDirective(std::string str, std::istringstream &iss)
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
			checkDuplicate(str, this->_check.loc.dup);
			checkLocationArgCount(i, iss);
			return (0);
		}
	}
	/*debug*/ std::cout << PINK << "errorLocationDirective invalid: " << str << RESET << std::endl;
    return (1);
}

// continue here
void	Config::errorCheckLocation(std::istringstream &iss, std::ifstream &inFile)
{
	std::string buffer, word;

	this->_check.loc.dup.clear();
	this->_check.loc.brace.clear();
	this->_check.loc.b_openBrace = 0;
	this->_check.loc.b_closeBrace = 0;

	/* checks location base arg_count */
	errorLocationBase(iss);

	/* checks arg_count in location scope{} */
	while (std::getline(inFile, buffer))
	{
		iss.clear();
		iss.str(buffer);
		this->_check.line_count++;

		if (!(iss >> word) || ignoreKeyword(word, iss, this->_check.loc))
			continue ;
		this->_check.keyword = word;

		/* updates braces && throw if no appropriate open/close brace */
		if (this->_check.loc.b_openBrace == 0)
			this->_check.loc.b_openBrace = checkBraces("{", this->_check.loc.brace);
		if (word == "}" && noMoreBrace(iss))
			return ;
		if (errorCommonDirective(word, iss, this->_check.loc.dup) && errorLocationDirective(word, iss))
        	throw (std::invalid_argument(ERR_DIRECTIVEINVALID));
	}
	throw std::invalid_argument(ERR_ARGCOUNTINVALID);
}