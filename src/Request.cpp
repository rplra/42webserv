#include "Request.hpp"

Request::Request() 
:
	_socket_fd(0),
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

Request::Request(int fd) 
:
	_socket_fd(fd),
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
	return (_method);
}

const	std::string& Request::getPath() const
{
	return (_path);
}

const	std::map<std::string, std::string>&	Request::getHeaders() const
{
	return (_headers);
}

const	std::string& Request::getBody() const
{
	// std::cout << "get body" << std::endl; // DEBUG
	return (_body);
}

const 	std::map<std::string, std::string>&	Request::getCookies() const
{
	return (_cookies);
}

const	ParserState& Request::getState() const
{
	return (_state);
}

bool	Request::hasBody()
{
	return (_state == PARSE_BODY);
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
				break;
			
			case PARSE_HEADERS:
				parseHeaders(_raw, _parsed_pos);
				decideBodyState();
				break;

			case PARSE_BODY:
			std::cout << "> state: PARSE_BODY" << std::endl; // DEBUG
			parseBody(_raw, _parsed_pos);
				break;

			case PARSE_BODY_CONTENT_LENGTH:
			parseContentLengthBody(_raw, _parsed_pos);
				break;
			
			case PARSE_BODY_CHUNKED_SIZE:
			case PARSE_BODY_CHUNKED_DATA:
			parseChunkedBody(_raw, _parsed_pos);
			std::cout << "> state: PARSE_BODY_CHUNKED" << std::endl; // DEBUG
				break;

			// case PARSE_BODY_COMPLETE:
			// std::cout << "> state: PARSE_BODY_COMPLETE" << std::endl; // DEBUG
			case PARSE_COMPLETE:
			std::cout << "> state: PARSE_COMPLETE" << std::endl; // DEBUG
			case PARSE_ERROR: // later response will check state > build error
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

	validateRequestLine();
	pos = line_end + 2; // move past "\r\n"
	_state = PARSE_HEADERS;
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
		key = toLower(key);
		std::string value = trim(std::string(&raw[colon + 1], line_end - (colon + 1)));
		// std::cout << "\n> KEY:VALUE : " << key << " : " << value; // DEBUG

		_headers[key] = value;
		handleSpecialHeaders(key, value);
		pos = line_end + 2;
	}
	validateHeaders();
	pos = headers_end + 4; // move cursor to body_start, skipping header_end empty line
}

void	Request::parseCookies(const std::string& value)
{
	size_t start = 0;
	size_t val = start + std::strlen("session_id=");
	size_t end = value.size();

	_cookies["session_id"] = std::string(&value[val], end - val);

	std::cout << "\n\n> parse cookies: " << value << std::endl; // DEBUG
	std::cout << "> session_id : " << _cookies["session_id"] << "\n" << std::endl; // DEBUG
}

void	Request::parseBody(const std::string& raw, size_t &pos)
{
	// body starts after \r\n\r\n (header_end + empty line)
	if (pos >= raw.length())
		return; // no body data yet
	if (_isChunked)
	{
		_state = PARSE_BODY_CHUNKED_SIZE;
		parseChunkedBody(raw, pos);
	}
	else if (_content_length > 0)
	{
		// std::cout << "> check length > 0" << std::endl; // DEBUG
		_state = PARSE_BODY_CONTENT_LENGTH;
		parseContentLengthBody(raw, pos);
	}
	// else if (_content_length == 0)
	// 	_state = PARSE_COMPLETE;
}

void	Request::parseChunkedBody(const std::string& raw, size_t &pos)
{
	size_t current_chunk_size = 0;

	while (pos < raw.length())
	{
		// read chunk size
		if (_state == PARSE_BODY_CHUNKED_SIZE)
		{
			// look for line end
			size_t line_end = raw.find("\r\n", pos);
			if (line_end == std::string::npos)
				return ;

			std::string hex;
			// extract hex size string
			size_t semicolon = raw.find(';', pos);
			if (semicolon == std::string::npos || semicolon > line_end)
				hex = std::string(&raw[pos], line_end - pos);
			else
				hex = std::string(&raw[pos], semicolon - pos);
			
			// convert hex to decimal
			current_chunk_size = std::strtoul(hex.c_str(), NULL, 16);
			pos = line_end + 2;

			// check for last chunk
			if (current_chunk_size == 0)
			{
				size_t trailer_end = raw.find("\r\n\r\n", pos);
				if (trailer_end != std::string::npos)
					pos = trailer_end + 4;
				std::cout << "> last chunk" <<std::endl; // DEBUG
				_state = PARSE_COMPLETE;
				return;
			}

			// move on to reading data
			// pos = line_end + 2;
			_state = PARSE_BODY_CHUNKED_DATA;
			std::cout << "> keep reading data" <<std::endl; // DEBUG
		}

		// read chunk data
		if (_state == PARSE_BODY_CHUNKED_DATA)
		{
			size_t available = raw.length() - pos;
			if (available < current_chunk_size + 2) // \r\n
				return;
			_body.append(raw, pos, current_chunk_size);

			// check size limit
			if (_body.length() > client_max_body_size)
			{
				_error_code = HTTP_PAYLOAD_TOO_LARGE;
				_state = PARSE_ERROR;
				return ;
			}
			pos += current_chunk_size + 2;
			_state = PARSE_BODY_CHUNKED_SIZE;
		}
	}
}

void	Request::parseContentLengthBody(const std::string& raw, size_t &pos)
{
	// calculate how much data is available (from buffered _raw)
	// std::cout << "> parse content body" << std::endl; // DEBUG
	size_t	available = _raw.length() - pos;

	// calculate how much more we need (to meet content_length)
	size_t	body_received = _body.length();
	size_t	body_remaining = _content_length - body_received;

	// case a: not enough data
	if (available < body_remaining)
	{
		_body.append(raw, pos, available);
		pos += available;
		_state = PARSE_BODY_CONTENT_LENGTH;
		return;
	}
	
	// case b: body completes / extra data
	_body.append(raw, pos, body_remaining);
	// std::cout << "> " << _body << std::endl; // DEBUG
	pos += body_remaining;
	_state = PARSE_COMPLETE;
}

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

void	Request::validateHeaders()
{
	// Chunked + Content-Length together → 400 Bad Request
	if (_isChunked && _content_length != 0)
	{
		_error_code = HTTP_BAD_REQUEST;
		_state = PARSE_ERROR;
		return;
	}
	// POST without body headers → 411 Length Required
	if (_method == "POST" && !_isChunked && _content_length == 0)
	{
		_error_code = HTTP_LENGTH_REQUIRED;
		_state = PARSE_ERROR;
		return;
	}
}

void	Request::handleSpecialHeaders(const std::string& key, const std::string& value)
{
	if (key == "content-length")
		_content_length = std::stoul(value);
	else if (key == "transfer-encoding" && value == "chunked")
		_isChunked = true; 
	else if (key == "cookie")
		parseCookies(value);
}

void	Request::decideBodyState()
{
	if (_isChunked || _content_length > 0)
		_state = PARSE_BODY;
	else
		_state = PARSE_COMPLETE;
}