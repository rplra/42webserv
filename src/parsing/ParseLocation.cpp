#include "Utils.hpp"
#include "Config.hpp"
#include "ConfigParse.hpp"

void	Server::parseLocationDirective(std::size_t code, std::istringstream &iss, Location &data)
{
	// std::string word;

	switch (code)
	{
		case CGI_HANDLER:
			assignCgiContainer(data._cgi, iss);
			break;

		case ALLOWED_METHODS:
			assignVecContainer(data._allowed_methods, iss);
			break;

		case UPLOAD_STORE:
			iss >> data._upload_path;
			// data._upload_path = trimStringTail(word, ';');
			break;

		case RETURN:
			assignMapContainer(data._redirect, iss);
			break;

		case ALIAS:
			iss >> data._alias;
			break;

		default:
			break;
	}
}

bool	Server::handleLocationDirective(std::string str, std::istringstream &iss, Location& data)
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
			parseLocationDirective(i, iss, data);
			return (1);
		}
	}
	return (0);
}
