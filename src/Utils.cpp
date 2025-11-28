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
