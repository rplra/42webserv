#include "Webserv.hpp"

// request
void testRequestWithoutBody();
void testRequestContentLengthBody();
void testRequestChunkedBody();

// response
Request* createRequestWithoutBody();
void testResponseStatic(Request* request, Server& server);
void testResponseError(Request* request, Server& server);
void testResponseAutoIndex(Request* request, Server& server);
void testResponseRedirect(Request* request, Server& server);


/*********************************  MAIN TEST  *********************************/
int main ()
{
	// testRequestWithoutBody();
	// testRequestContentLengthBody();
	// testRequestChunkedBody();
	// TO DO: to handle edge cases from chrome

	Server	server;

	// Request* request1 = createRequestWithoutBody();
	// testResponseStatic(request1, server);
	// testResponseError(request1, server);
	// delete request1;

	Request* request2 = new Request();
	std::string testpath = "GET /test/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
	request2->readRequest(testpath);
	// testResponseAutoIndex(request2, server);
	testResponseRedirect(request2, server);
	delete request2;

	return (0);
}


/*********************************  TEST REQUEST  *********************************/
void testRequestWithoutBody()
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

	// std::cout << "\nCookies: " << std::endl;
	// for (std::map<std::string, std::string>::const_iterator it = test.getCookies().begin(); it != test.getCookies().end(); ++it)
	// 	std::cout << it->first << ":" << it->second << std::endl;

	std::cout << CYAN << "\nParse Complete? " << RESET << (test.isParseComplete() ? "Yes" : "No") << std::endl;
	std::cout << std::endl;
}

void testRequestContentLengthBody()
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
	
	// std::cout << "\nCookies: " << std::endl;
	// for (std::map<std::string, std::string>::const_iterator it = test.getCookies().begin(); it != test.getCookies().end(); ++it)
	// 	std::cout << it->first << ":" << it->second << std::endl;

	std::cout << ORANGE << "\nBody: " << RESET << std::endl;
	std::cout << test.getBody() << std::endl;
	// std::cout << "here" << std::endl;

	std::cout << CYAN << "\nParse Complete? " << RESET << (test.isParseComplete() ? "Yes" : "No") << std::endl;
	std::cout << std::endl;
}

void testRequestChunkedBody()
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

	// std::cout << "\nCookies: " << std::endl;
	// for (std::map<std::string, std::string>::const_iterator it = test.getCookies().begin(); it != test.getCookies().end(); ++it)
	// 	std::cout << it->first << ":" << it->second << std::endl;

	std::cout << ORANGE << "\nBody: " << RESET << std::endl;
	std::cout << test.getBody() << std::endl;

	std::cout << CYAN << "\nParse Complete? " << RESET << (test.isParseComplete() ? "Yes" : "No") << std::endl;
	std::cout << std::endl;
}


/*********************************  TEST RESPONSE  *********************************/
Request* createRequestWithoutBody()
{
    std::string testRequest = 
    "GET /index.html HTTP/1.1\r\n"
    "Host: localhost:8080\r\n"
    "User-Agent: TestClient/1.0\r\n"
    "Accept: text/html\r\n"
    "Cookie: session_id=abc123\r\n\r\n";

    Request* req = new Request(); 
    req->readRequest(testRequest);
    return req;
}

void testResponseStatic(Request* request, Server& server)
{
	Response test(request, server, HTTP_OK);
	
	test.setType(STATIC);
	test.file_path = "./www/index.html";

	std::cout << PURPLE << ">> HTTP Response: Static" << RESET << std::endl;
	test.buildResponse();
	// /* debug */std::cout << "> printing raw response" << std::endl;
	// std::cout << raw_response << std::endl;
}

void testResponseError(Request* request, Server& server)
{
	Response test(request, server, HTTP_NOT_FOUND);

	test.setType(ERROR);
	std::cout << PURPLE << ">> HTTP Response: Error" << RESET << std::endl;
	test.buildResponse();
}

void testResponseAutoIndex(Request* request, Server& server)
{
	Response test(request, server, HTTP_OK);

	test.setType(STATIC);
	test.file_path = "./www/test";
	std::cout << PURPLE << ">> HTTP Response: Autoindex" << RESET << std::endl;
	test.buildResponse();
}

void testResponseRedirect(Request* request, Server& server)
{
	Response test(request, server, HTTP_OK);

	test.setType(REDIRECT);
	test.file_path = "./www/test";
	std::cout << PURPLE << ">> HTTP Response: Redirect" << RESET << std::endl;
	test.buildResponse();
}