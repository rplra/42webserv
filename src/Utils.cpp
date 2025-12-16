#include "Webserv.hpp"

void	checkArgument(int ac)
{
	if (ac != 2)
		throw std::invalid_argument(ERR_ARGFORMAT);
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

// // RFC 3986 — Uniform Resource Identifier (URI): Generic Syntax
// //std::string	normalizePath(const std::string& s);
// // {
// 	// percent decoding - %xx (%20 == space, %2F == /, %2E == . , etc)
// 	// split into segments (parts btw '/' - eg; /a/b/../c → segments: ["a", "b", "..", "c"])
// 	// path traversal ("..", '.') (remove dot segments - eg; /a/b/../c/./d → /a/c/d)
// 	// rebuild a canonical path (After normalization, join the segments back with /)
// // }


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

/* 
	eg : GET /search/photos%20gallery?user=John+Doe&file=report%202025.pdf HTTP/1.1
	_path = /search/photos%20gallery
	_query = user=John+Doe&file=report%202025.pdf
	John+Doe > John Doe
	report%202025.pdf > report 2025.pdf

	'+' 	; spaces are encoded as '+' with HTML form submission
	'%20' 	; spaces are encoded as '%20' with URL (browser / manual encoding)
*/
std::string	urlDecode(const std::string& s)
{
	std::string result;
	result.reserve(s.length());

	for (size_t i = 0; i < s.length(); ++i)
	{
		if (s[i] == '%' && i + 2 < s.length())
		{
			// decode %XX hex encoding
			char hex[3] = { s[i + 1], s[i + 2], 0};	// [hex digit, hex digit, null]
			char *end;								// points to first char not used in conversion
			long value = std::strtol(hex, &end, 16);

			if (*end == 0)
			{
				result += static_cast<char>(value);
				i += 2;
				continue;
			}
		}
		else if (s[i] == '+')
		{
			// '+' represents space in query strings
			result += ' ';
			continue;
		}
		result += s[i];
	}
	return (result);
}

void handleSignal(int signum)
{
    g_signal = 0;
    std::cout << RED << "\nSignal " << signum << " received, shutting down server..." << RESET << std::endl;
}

bool isDirectory(const std::string& path)
{
    struct stat pathStat;
    if (stat(path.c_str(), &pathStat) != 0)
        return false; // error accessing path
    return S_ISDIR(pathStat.st_mode);
}

bool isFile(const std::string& path)
{
    struct stat pathStat;
    if (stat(path.c_str(), &pathStat) != 0)
        return false; // error accessing path
    // Check if it's a regular file
    return S_ISREG(pathStat.st_mode);
}