#include "Webserv.hpp"

/*
   keywords referenced:
   global, server & location context
   https://www.alimnaqvi.com/blog/webserv
 */

/* read up to ; only */

void	parseConfig(char **av)
{
	std::ifstream inFile(av[1]);

	try
	{
		if (!inFile.is_open())
			throw (std::invalid_argument(ERR_FILEINVALID));
		if (inFile.peek() == EOF)
			throw (std::invalid_argument(ERR_FILEEMPTY));
		
		/* else, start parsing */
		std::string buffer;
		while (std::getline(inFile, buffer))
		{
			std::istringstream	iss(buffer);
			std::cout << buffer << std::endl;
			// std::string word;
			// while (iss >> word)
				// std::cout << word << std::endl;
		}
		/* reset after reading */
		inFile.close();
		return ;
	}
	catch (std::exception &err)
	{
		std::cout	<< RED
					<< "Exception: " << err.what()
					<< RESET << std::endl;
	}
	exit(1);
}