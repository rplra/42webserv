#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include "Webserv.hpp"

enum	ParserState
{
	PARSE_REQUEST_LINE,
	PARSE_HEADERS,
	PARSE_BODY_CONTENT_LENGTH,
	PARSE_BODY_CHUNKED_SIZE,
	PARSE_BODY_CHUNKED_DATA,
	PARSE_BODY_COMPLETE,
	PARSE_COMPLETE,
	PARSE_ERROR
};

class	Request
{
public:
	Request();
	~Request() {};

	// void	readRequest(int fd);
	void	readRequest(std::string& request); // append; incremental reading

	// getters
	const	std::string&						getMethod() const;
	const	std::string&						getPath() const;
	const	std::map<std::string, std::string>&	getHeaders() const;
	const	std::string&						getBody() const;
	const 	std::map<std::string, std::string>&	getCookies() const;
	const	ParserState							getState() const; // only for test

	bool	hasBody();
	bool 	hasCookies();
	bool	isParseComplete();

private:
	std::string							_raw;
	std::string							_method;
	std::string							_path;
	std::string							_http_version;
	std::map<std::string, std::string>	_headers;
	size_t								_content_length;
	std::string							_body;
	std::map<std::string, std::string>	_cookies; // session_id
	
	ParserState							_state;
	size_t								_parsed_pos;
	bool								_isChunked;

	void	parseByState();
	void 	parseRequestLine(const std::string& requestLine);
	void 	parseHeaders(const std::string& header);
	void	parseBody(const std::string& body); // need to handle content length & chunked
	size_t	bodyPosition(const std::string& request);
};



#endif
