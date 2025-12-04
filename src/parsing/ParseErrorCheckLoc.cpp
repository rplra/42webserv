#include "Utils.hpp"
#include "Config.hpp"
#include "ConfigParse.hpp"

void	Server::checkLocationArgCount(size_t code, std::istringstream &iss)
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
bool	Server::errorLocationDirective(std::string str, std::istringstream &iss)
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
			checkLocationArgCount(i, iss);
			return (0);
		}
	}
	/*debug*/ std::cout << PINK << "errorLocationDirective invalid: " << str << RESET << std::endl;
    return (1);
}

// continue here
bool	Server::errorLocationArgCount(std::istringstream &iss, std::ifstream &inFile)
{
	std::string buffer, word;
	int			count = 0;

	while (iss >> word)
	{
		std::cout << "errorLocationArgCount " << word << std::endl;
		if (word == "{")
			break ;
		count++;
	}
	/*debug*/ std::cout << ", loc_arg_count: " << count << std::endl;
	if (count != 1)
		throw (std::invalid_argument(ERR_ARGCOUNTINVALID));
	while (std::getline(inFile, buffer))
	{
		iss.clear();
		iss.str(buffer);
		if (!(iss >> word))
			continue ;
		if (word == "}")
			return (0);
		if (errorCommonDirective(word, iss) && errorLocationDirective(word, iss))
        	throw (std::invalid_argument(ERR_DIRECTIVEINVALID));
	}
	return (1);
}