// #include <iostream>
// #include <vector>

// #include "Macros.hpp"
// #include "Utils.hpp"
#include "Webserv.hpp"

void	checkArgument(int ac)
{
	try
	{
		if (ac != 2)
			throw std::invalid_argument(ERR_ARGFORMAT);
		return ;
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << "Exception: " << e.what() << RESET << std::endl;
	}
	exit (1);
}

/* trims and discards string after symbol specified */
std::string	trimStringTail(const std::string &str, char c)
{
	std::size_t i = str.find(c);

	if (i != std::string::npos)
		return (str.substr(0, i));
	return (str);
}

/* trims and discards string before symbol specified */
std::string	trimStringHead(const std::string &str, char c)
{
	std::size_t i = str.find(c);

	if (i != std::string::npos)
		return (str.substr(i + 1, std::string::npos));
	return (str);
}