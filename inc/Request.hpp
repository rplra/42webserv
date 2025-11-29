#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include "Webserv.hpp"

enum	ParserState
{
	PARSE_REQUEST_LINE,
	PARSE_HEADERS,
	PARSE_BODY,
	PARSE_BODY_CONTENT_LENGTH,
	PARSE_BODY_CHUNKED_SIZE,
	PARSE_BODY_CHUNKED_DATA,
	// PARSE_BODY_COMPLETE,
	PARSE_COMPLETE,
	PARSE_ERROR
};

class	Request
{
public:
	Request();
	~Request() {};
	// explicit Request(int socket_fd); // fd should only belong to client (network)
	
	// void	readRequest(int fd);
	void	readRequest(std::string& request); // append; incremental reading

	//		getters
	const	std::string&						getMethod() const;
	const	std::string&						getPath() const;
	const	std::map<std::string, std::string>&	getHeaders() const;
	const	std::string&						getContentType() const;
	const	std::string&						getConnection() const;
	const	std::string&						getSessionID() const;
	// const 	std::map<std::string, std::string>&	getCookies() const;
	const	std::string&						getBody() const;
	const	ParserState&						getState() const;

	bool	hasBody();
	bool 	hasCookies() const;
	bool	isParseComplete();

private:
	Request(const Request& src);
	Request& operator=(const Request& src);

	std::string							_raw;
	std::string							_method;
	std::string							_path;
	std::string							_http_version;
	std::map<std::string, std::string>	_headers;
	size_t								_content_length;
	std::string							_content_type;
	std::string							_body;
	std::map<std::string, std::string>	_cookies; // may be redundant
	std::string							_session_id;
	std::string							_connection;
	
	ParserState							_state;
	size_t								_parsed_pos;
	int									_error_code;
	bool								_isChunked;

	// post: multipart - content type

	//		parser
	void	parseByState();
	void 	parseRequestLine(const std::string& raw, size_t &pos);
	void 	parseHeaders(const std::string& raw, size_t &pos);
	void	parseCookies(const std::string& value);
	void	parseBody(const std::string& raw, size_t &pos);
	void	parseContentLengthBody(const std::string& raw, size_t &pos);
	void	parseChunkedBody(const std::string& raw, size_t &pos);

	// 		helpers
	bool	isValidPath();
	void	validateRequestLine();
	void	validateHeaders();
	void	handleSpecialHeaders(const std::string& key, const std::string& value);
	void	decideBodyState();
	// void		readChunkedSize(const std::string& raw, size_t &pos);
	// void		readChunkedData(const std::string& raw, size_t &pos);

};



#endif
