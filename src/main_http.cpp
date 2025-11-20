#include "Webserv.hpp"

// test http stuff
int main (int ac, char **av)
{
	(void) av;
	(void) ac;

	// correct format according to protocol
	std::string testRequest = "GET /index.html HTTP/1.1\r\nHost:     localhost:8080\r\nUser-Agent: TestClient/1.0\r\nAccept: text/html\r\nCookie: session_id=abc123\r\n\r\n";
	
	// wrong http_version - to handle
	// std::string testRequest = "GET /index.html HTTP/1.0\r\nHost: localhost:8080\r\nUser-Agent: TestClient/1.0\r\nAccept: text/html\r\nCookie: session_id=abc123\r\n\r\n";

	Request test;
	std::cout << "\n>> HTTP Request: " << std::endl;
	test.readRequest(testRequest);

	std::cout << "\nMethod : " << test.getMethod() << std::endl;
	std::cout << "Path   : " << test.getPath() <<std::endl;
	
	std::cout << "\nHeaders: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = test.getHeaders().begin(); it != test.getHeaders().end(); ++it)
		std::cout << it->first << ":" << it->second << std::endl;
	
	std::cout << "\nCookies: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = test.getCookies().begin(); it != test.getCookies().end(); ++it)
		std::cout << it->first << ":" << it->second << std::endl;

	std::cout << "\nParse Complete? " << (test.isParseComplete() ? "Yes" : "No") << std::endl;

	return (0);
}