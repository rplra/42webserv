#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <string>
#include <map>
#include "Macros.hpp"

enum	ParserState
{
	PARSE_REQUEST_LINE,
	PARSE_HEADERS,
	PARSE_BODY,
	PARSE_BODY_CONTENT_LENGTH,
	PARSE_BODY_CHUNKED_SIZE,
	PARSE_BODY_CHUNKED_DATA,
	PARSE_COMPLETE,
	PARSE_ERROR
};

class	Request
{
public:
	Request();
	~Request() {};
	
	void	handleRequest(const char* data, size_t size, size_t limit);

	//		setters
	void										setStatus(HttpStatus status);
	void										setState(ParserState state);

	//		getters
	const	std::string&						getMethod() const;
	const	std::string&						getPath() const;
	const	std::string&						getQuery() const;
	const	std::string&						getQueryEntry(const std::string& key) const;
	const	std::map<std::string, std::string>&	getQueryEntries() const;
	const	std::string&						getHttpVersion() const;
	const	std::string&						getHeader(const std::string& key) const;
	const	std::map<std::string, std::string>&	getHeaders() const;
	size_t										getContentLength() const;
	const	std::string&						getContentType() const;
	const	std::string&						getConnection() const;
	const	std::string&						getSessionID() const;
	const	std::string&						getBody() const;
	const	ParserState&						getState() const;
	HttpStatus									getStatus() const;

	bool	hasBody();
	bool	hasMultipart() const;
	bool 	hasSessionId() const;
	bool	isParseComplete();
	void	clear();

private:
	Request(const Request& src);
	Request& operator=(const Request& src);

	std::string							_raw;
	std::string							_method;
	std::string							_path;
	std::string							_query;
	std::map<std::string, std::string>	_query_entries;				
	std::string							_http_version;
	std::map<std::string, std::string>	_headers;
	size_t								_content_length;
	std::string							_content_type;
	std::string							_body;
	std::map<std::string, std::string>	_cookies;
	std::string							_session_id;
	std::string							_connection;
	
	ParserState							_state;
	size_t								_parsed_pos;
	HttpStatus							_status;
	bool								_isChunked;
	size_t								_current_chunk_size;

	//		parser
	void	parseByState(size_t limit);
	void 	parseRequestLine(const std::string& raw, size_t &pos);
	void	parseQuery(const std::string& query);
	void 	parseHeaders(const std::string& raw, size_t &pos);
	void	parseBoundary();
	void	parseCookies(const std::string& value);
	void	parseBody(const std::string& raw, size_t &pos, size_t limit);
	void	parseContentLengthBody(const std::string& raw, size_t &pos, size_t limit);
	void	parseChunkedBody(const std::string& raw, size_t &pos, size_t limit);

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
