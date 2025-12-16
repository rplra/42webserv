#ifndef __DEBUG_HPP__
# define __DEBUG_HPP__

#include "Config.hpp"

template <typename T>
void	printVectorContainer(std::vector<T> &data)
{
	typename std::vector<T>::iterator it = data.begin();
	typename std::vector<T>::iterator ite = data.end();

	while (it != ite)
	{
		std::cout << GREY << "  " << *it << RESET << std::endl;
		it++;
	}
}

template <typename R, typename T>
void	printMapContainer(std::map<R, T> data)
{
	typename std::map<R, T>::iterator it = data.begin();
	typename std::map<R, T>::iterator ite = data.end();

	while (it != ite)
	{
		std::cout	<< GREY << "  " << it->first << " = " << it->second
					<< RESET << std::endl;
		it++;
	}
}

template <typename T>
void	printLocations(std::vector<T> &data)
{
	typename std::vector<T>::iterator it	= data.begin();
	typename std::vector<T>::iterator ite	= data.end();

	while (it != ite)
	{
		std::cout << "  path            : " << it->_path << std::endl;
		std::cout << "  root            : " << it->_root << std::endl;
		std::cout << "  index           : " << it->_index << std::endl;
		std::cout << "  autoindex       : " << it->_autoindex << std::endl;
		std::cout << "  client_body     : " << it->_client_max_body_size << std::endl;
		std::cout << "  error_pages     : " << std::endl;
		printMapContainer(it->_error_pages);
		std::cout << "  allowed_methods : " << std::endl;
		printVectorContainer(it->_allowed_methods);
		std::cout << "  cgi             : " << std::endl;
		printMapContainer(it->_cgi);
		std::cout << "  upload_path     : " << it->_upload_path << std::endl;
		std::cout << "  redirect        : " << std::endl;
		printMapContainer(it->_redirect);
		std::cout << std::endl;
		it++;
	}
}

#endif