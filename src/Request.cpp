#include "Request.hpp"

Request::Request() 
:
	_raw(),
	_method(),
	_path(),
	_http_version(),
	_headers(),
	_content_length(0),
	_body(),
	_cookies(),
	_state(PARSE_REQUEST_LINE),
	_parsed_pos(0),
	_isChunked(false)
{}

// helper func since we dont have socket yet
ssize_t _recv(std::string& src, char* buffer, size_t size)
{
	if (src.empty())
		return (0);
	ssize_t n = std::min(size, src.size());
	std::memcpy(buffer, src.data(), n);
	src.erase(0, n);
	return n;
}

// to replace request with fd
void	Request::readRequest(std::string& request)
{
	char buffer[BUFFER_SIZE];

	// ssize_t n = recv(fd, buffer, BUFFER_SIZE, 0); // use when socket ready
	ssize_t n = _recv(request, buffer, BUFFER_SIZE);
	if (n > 0)
	{
		_raw.append(buffer, n);
		// std::cout << _raw; // DEBUG
		parseByState();
	}
}

const	std::string& Request::getMethod() const
{
	return _method;
}

const	std::string& Request::getPath() const
{
	return _path;
}

const	std::map<std::string, std::string>&	Request::getHeaders() const
{
	return _headers;
}

const	std::string& Request::getBody() const
{
	return _body;
}

const 	std::map<std::string, std::string>&	Request::getCookies() const
{
	return _cookies;
}

bool	Request::hasBody()
{
	return(_state == PARSE_BODY_COMPLETE);
}
	
bool 	Request::hasCookies()
{
	return (!_cookies.empty());
}

bool	Request::isParseComplete()
{
	return (_state == PARSE_COMPLETE);
}

void	Request::parseByState()
{
	// std::cout << _parsed_pos << std::endl; // DEBUG
	// std::cout << _raw.size() << std::endl; // DEBUG
	while (_parsed_pos < _raw.size())
	{
		switch (_state)
		{
			case PARSE_REQUEST_LINE:
				// std::cout << "here" << std::endl; // DEBUG
				parseRequestLine(_raw, _parsed_pos);
				_state = PARSE_HEADERS;
				break;
			
			case PARSE_HEADERS:
				parseHeaders(_raw, _parsed_pos);
				if (_content_length > 0)
					_state = PARSE_BODY_CONTENT_LENGTH;
				else if (_isChunked)
					_state = PARSE_BODY_CHUNKED_SIZE;
				else
					_state = PARSE_COMPLETE;
				break;

			case PARSE_BODY_CONTENT_LENGTH:
			// 	parseBody(_raw.substr(_parsed_pos));
			// 	if (_body.size() >= _content_length)
			// 		_state = PARSE_COMPLETE;
			// 	break;
			
			case PARSE_BODY_CHUNKED_SIZE:
			// 	// read chunk size, update pos
			// 	_state = PARSE_BODY_CHUNKED_DATA;
			// 	break;

			case PARSE_BODY_CHUNKED_DATA:
			// 	// read chunk of known size
			// 	// if last chuck is size 0
			// 		_state = PARSE_BODY_COMPLETE;
			// 	// else
			// 		_state = PARSE_BODY_CHUNKED_SIZE; // next chunk
			// 	break;

			case PARSE_BODY_COMPLETE:
			// 	_state = PARSE_COMPLETE;
			// 	break;

			case PARSE_COMPLETE:
			case PARSE_ERROR:
				return;
		}
	}
}

void 	Request::parseRequestLine(const std::string& raw, size_t &pos)
{
	// std::cout << "here" << std::endl; // DEBUG
	size_t line_end = raw.find("\r\n", pos);
	if (line_end == std::string::npos)
		return;
	
	size_t method_end = raw.find(' ', pos);
	if (method_end == std::string::npos || method_end > line_end)
		return;

	size_t path_end = raw.find(' ', method_end + 1);
	if (path_end == std::string::npos || path_end > line_end)
		return;

	_method = std::string(&raw[pos], method_end - pos);
	_path = std::string(&raw[method_end + 1], path_end - method_end - 1);
	_http_version = std::string(&raw[path_end + 1], line_end - path_end - 1);

	pos = line_end + 2; // move past "\r\n"
}

void 	Request::parseHeaders(const std::string& raw, size_t &pos)
{
	size_t headers_end = raw.find("\r\n\r\n");
	if (headers_end == std::string::npos)
		return;
	
	while (pos < headers_end)
	{
		size_t line_end = raw.find("\r\n", pos);
		if (line_end == std::string::npos || line_end > headers_end)
			return;

		size_t colon = raw.find(':', pos);
		if (colon == std::string::npos || colon > line_end)
			return;
		
		std::string key = trim(std::string(&raw[pos], colon - pos));
		std::string value = trim(std::string(&raw[colon + 1], line_end - (colon + 1)));
		// std::cout << "\n> KEY:VALUE : " << key << " : " << value; // DEBUG

		_headers[key] = value;
		handleSpecialHeaders(key, value);

		pos = line_end + 2;
	}
}

void	Request::parseCookies(const std::string& value)
{
	size_t start = 0;
	size_t val = start + std::strlen("session_id=");
	size_t end = value.size();

	_cookies["session_id"] = std::string(&value[val], end - val);

	std::cout << "\n\nparse cookies: " << value << std::endl; // DEBUG
	std::cout << "cookie value : " << _cookies["session_id"] << std::endl; // DEBUG
}

// void	Request::parseBody(const std::string& raw, size_t &pos) // need to handle content length & chunked
// {

// }

// size_t	Request::bodyPosition(const std::string& request)
// {

// }


/* 
	check for c <= 31 || c == 127 is to abide RFC 9112 (HTTP/1.1)
		- HTTP messages must consist of printable ASCII characters (0x20–0x7E) plus CRLF.
		- Control characters (0x00–0x1F, except \r and \n) MUST NOT appear in the request line or header fields. 
*/
bool	Request::isValidPath()
{
	if (_path.empty() || _path[0] != '/')
		return (false);

	for (size_t i = 0; i < _path.size(); i++)
	{
		char c = _path[i];

		if (c <= 31 || c == 127 || c == ' ' || c == '\\')
			return (false);
	}
	return (true);
}

void	Request::validateRequestLine()
{
	if ((_method != "GET" && _method != "POST" && _method != "DELETE") 
		|| !isValidPath() || _http_version != "HTTP/1.1")
		_state = PARSE_ERROR;
	return;
}

void	Request::handleSpecialHeaders(const std::string& key, const std::string& value)
{
	if (key == "Content-Length")
		_content_length = std::stoul(value);
	if (key == "Transfer-Encoding" && value == "chunked")
		_isChunked = true;
	if (key == "Cookie")
		parseCookies(value);
}