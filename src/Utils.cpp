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

// based on RFC 3986 — Uniform Resource Identifier (URI): Generic Syntax
//std::string	normalizePath(const std::string& s);
// {
	// percent decoding - %xx (%20 == space, %2F == /, %2E == . , etc)
	// split into segments (parts btw '/' - eg; /a/b/../c → segments: ["a", "b", "..", "c"])
	// path traversal ("..", '.') (remove dot segments - eg; /a/b/../c/./d → /a/c/d)
	// rebuild a canonical path (After normalization, join the segments back with /)
// }


// trims whitepsace
std::string trim(const std::string& s)
{
	size_t start = s.find_first_not_of(" \t");
	if (start == std::string::npos)
		return ("");
	size_t end = s.find_last_not_of(" \t");
	return (s.substr(start, end - start + 1));
}