#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__

#include "Webserv.hpp"
#include "Macros.hpp"

class Request;
class Server;

enum	ResponseType
{
	REDIRECT,
	STATIC,
	AUTOINDEX,
	CGI,
	ERROR
};

class Response
{
public:
	Response(const Request* request, const Server& server, HttpStatus status);
	// Response(HttpStatus status_code, const Server& server);
	~Response() {};

	std::string	buildResponse();
	bool		isResponseReady() const;

	std::string	getRawResponse();

	// TEMPORARY - until routing is implemented
	void		setType(ResponseType type);
	void		setError(HttpStatus code);
	std::string file_path;		// this should be private later
	std::string redirect_path; 	// this should be private later

private:
	Response();
	Response(const Response& src);
	Response& operator=(const Response& src);

	std::string							_http_version;
	HttpStatus							_status_code;
	std::string							_reason_phrase;
	std::map<std::string, std::string>	_headers;
	// std::string							_session_id;
	std::string							_body;
	std::string							_content_type;

	ResponseType						_type;
	std::map<std::string, std::string>	_mime;
	std::string							_raw_response;
	bool								_isBuilt;

	const Request*						_request;
	const Server&						_server;

	// setters
	void	setStatus(HttpStatus status);
	void	setHeader(const std::string& key, const std::string& value);
	void	setHeaders();
	void	setBody(const std::string& body);

	// builders
	void	buildRedirect();
	void	buildStatic();
	void	buildAutoIndex();
	// void	buildCgi();
	void	buildError(); 

	// helpers
	std::string getDate();
	std::string	getLastModified();
	std::string getMimeType(const std::string& path);

	// static
	std::string	getFileBody(const std::string& file_path);
	void		handleDirectory();
	void		serveFile(const std::string& file_path, HttpStatus status);

	// page generators
	std::string	generateAutoIndexBody(const std::string& file_path);
	// std::string	generateErrorPage(HttpStatus status);

};


#endif