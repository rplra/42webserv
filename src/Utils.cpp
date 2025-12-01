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

void handleSignal(int signum) {
    g_signal = 0;
    std::cout << RED << "\nSignal " << signum << " received, shutting down server..." << RESET << std::endl;
}

void closeAllFd(std::vector<pollfd>& fds) {
    std::cout << RED << "Closing all file descriptors..." << RESET << std::endl;
    for (size_t i=0; i < fds.size(); i++) {
        close(fds[i].fd);
    }
}

void closeClient(size_t& i, size_t& fds_count, std::vector<pollfd>& fds, std::map<int, 
    Request>& clientRequests, std::map<int, int>& clientServerMap) {

    close(fds[i].fd);
    clientRequests.erase(fds[i].fd);
    clientServerMap.erase(fds[i].fd);
    fds.erase(fds.begin() + i);

    // adjust index after erasing element as the remaining elements shift left 
    i--;
    fds_count--;
}