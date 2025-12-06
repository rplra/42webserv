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
		if (word == "{")
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
			checkDuplicate(str, this->_check.dup_loc);
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
	this->_check.dup_loc.clear();

	/* checks location / arg_count */
	errorLocationBase(iss);

	/* checks arg_count in location scope{} */
	while (std::getline(inFile, buffer))
	{
		this->_check.line_count++;

		iss.clear();
		iss.str(buffer);
		if (!(iss >> word))
			continue ;
		this->_check.keyword = word;
		if (word == "}")
			return ;
		if (errorCommonDirective(word, iss, this->_check.dup_loc) && errorLocationDirective(word, iss))
        	throw (std::invalid_argument(ERR_DIRECTIVEINVALID));
	}
	throw std::invalid_argument(ERR_ARGCOUNTINVALID);
}