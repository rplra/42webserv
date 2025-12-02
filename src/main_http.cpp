#include "Webserv.hpp"

void testWithoutBody();
void testContentLengthBody();
void testChunkedBody();

int main ()
{
	// testWithoutBody();
	// testContentLengthBody();
	testChunkedBody();

	// TO DO: to handle edge cases from chrome

	return (0);
}

void testWithoutBody()
{
	std::string testRequest = 
	"GET /index.html HTTP/1.1\r\n"
	"Host:     localhost:8080\r\n"
	"User-Agent: TestClient/1.0\r\n"
	"Accept: text/html\r\n"
	"Cookie: session_id=abc123\r\n\r\n";

	Request test;
	std::cout << PURPLE << ">> HTTP Request: Test Without Body" << RESET << std::endl;
	test.readRequest(testRequest);

	std::cout << ORANGE << "\nRequest Line: " << RESET << std::endl;
	std::cout << "Method : " << test.getMethod() << std::endl;
	std::cout << "Path   : " << test.getPath() <<std::endl;
	
	std::cout << ORANGE << "\nHeaders: " << RESET << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = test.getHeaders().begin(); it != test.getHeaders().end(); ++it)
		std::cout << it->first << ":" << it->second << std::endl;
	
	std::cout << "\nCookies: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = test.getCookies().begin(); it != test.getCookies().end(); ++it)
		std::cout << it->first << ":" << it->second << std::endl;

	std::cout << CYAN << "\nParse Complete? " << RESET << (test.isParseComplete() ? "Yes" : "No") << std::endl;
	std::cout << std::endl;
}

void testContentLengthBody()
{
	std::string contentRequest =
    "POST /submit HTTP/1.1\r\n"
    "Host: localhost:8080\r\n"
	"User-Agent: TestClient/1.0\r\n"
	"Accept: text/html\r\n"
    "Content-Length: 11\r\n"
	"Cookie: session_id=abc123\r\n"
    "\r\n"
    "Hello World"; // 11 bytes

	Request test;
	std::cout << PURPLE << ">> HTTP Request: Test with Content-Length Body" << RESET << std::endl;
	test.readRequest(contentRequest);

	std::cout << ORANGE << "\nRequest Line: " << RESET << std::endl;
	std::cout << "Method : " << test.getMethod() << std::endl;
	std::cout << "Path   : " << test.getPath() <<std::endl;
	
	std::cout << ORANGE << "\nHeaders: " << RESET << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = test.getHeaders().begin(); it != test.getHeaders().end(); ++it)
		std::cout << it->first << ":" << it->second << std::endl;
	
	std::cout << "\nCookies: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = test.getCookies().begin(); it != test.getCookies().end(); ++it)
		std::cout << it->first << ":" << it->second << std::endl;

	std::cout << ORANGE << "\nBody: " << RESET << std::endl;
	std::cout << test.getBody() << std::endl;
	// std::cout << "here" << std::endl;

	std::cout << CYAN << "\nParse Complete? " << RESET << (test.isParseComplete() ? "Yes" : "No") << std::endl;
	std::cout << std::endl;
}

void testChunkedBody()
{
	std::string chunkedRequest =
    "POST /upload HTTP/1.1\r\n"
    "Host: localhost:8080\r\n"
	"User-Agent: TestClient/1.0\r\n"
	"Accept: text/html\r\n"
    "Transfer-Encoding: chunked\r\n"
	"Cookie: session_id=abc123\r\n"
    "\r\n"		// empty line
    "5\r\n"		// start chunk
    "Hello\r\n"
    "6\r\n"
    " World\r\n"
    "0\r\n"		// last chunk
    "\r\n";		// end chunk

	Request test;
	std::cout << PURPLE << ">> HTTP Request: Test with Chunked Body" << RESET << std::endl;
	test.readRequest(chunkedRequest);

	std::cout << ORANGE << "\nRequest Line: " << RESET << std::endl;
	std::cout << "Method : " << test.getMethod() << std::endl;
	std::cout << "Path   : " << test.getPath() <<std::endl;
	
	std::cout << ORANGE << "\nHeaders: " << RESET << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = test.getHeaders().begin(); it != test.getHeaders().end(); ++it)
		std::cout << it->first << ":" << it->second << std::endl;
	
	std::cout << "\nCookies: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = test.getCookies().begin(); it != test.getCookies().end(); ++it)
		std::cout << it->first << ":" << it->second << std::endl;

	std::cout << ORANGE << "\nBody: " << RESET << std::endl;
	std::cout << test.getBody() << std::endl;

	std::cout << CYAN << "\nParse Complete? " << RESET << (test.isParseComplete() ? "Yes" : "No") << std::endl;
	std::cout << std::endl;
}