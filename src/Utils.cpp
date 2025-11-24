#include <iostream>

#include "Macros.hpp"
#include "Utils.hpp"

void	checkArgument(int ac)
{
	try
	{
		if (ac != 2)
			throw std::invalid_argument(ERR_ARGFORMAT);
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << "Exception: " << e.what() << RESET << std::endl;
	}
	exit (1);
}

// RFC 3986 — Uniform Resource Identifier (URI): Generic Syntax
//std::string	normalizePath(const std::string& s);
// {
	// percent decoding - %xx (%20 == space, %2F == /, %2E == . , etc)
	// split into segments (parts btw '/' - eg; /a/b/../c → segments: ["a", "b", "..", "c"])
	// path traversal ("..", '.') (remove dot segments - eg; /a/b/../c/./d → /a/c/d)
	// rebuild a canonical path (After normalization, join the segments back with /)
// }


// RFC 9112 §5.2 (Field Syntax) - leading/trailing OWS around the value is ignored
// trims whitepsace
std::string trim(const std::string& s)
{
	size_t start = s.find_first_not_of(" \t");
	if (start == std::string::npos)
		return ("");
	size_t end = s.find_last_not_of(" \t");
	return (s.substr(start, end - start + 1));
}

// RFC 9110 (Http Semantics; headers interpretation), RFC 9112 (Message Syntax; parse http msg correctly)
std::string toLower(const std::string& s)
{
	std::string res = s;
	for (size_t i = 0; i < res.length(); i++)
		res[i] = std::tolower(static_cast<unsigned char>(res[i]));
	return (res);
}