#include "Request.hpp"
#include "Utils.hpp"

Request::Request() 
:
	_raw(),
	_method(),
	_path(),
	_query(),
	_query_entries(),
	_http_version(),
	_headers(),
	_content_length(0),
	_body(),
	_cookies(),
	_session_id(),
	_connection("keep-alive"),
	_state(PARSE_REQUEST_LINE),
	_parsed_pos(0),
	_status(HTTP_OK),
	_isChunked(false),
	_current_chunk_size(0)
{
}

void	Request::handleRequest(const char* data, size_t size, size_t limit)
{
		_raw.append(data, size);
		parseByState(limit);
}

const	std::string& Request::getMethod() const
{
	return (_method);
}

const	std::string& Request::getPath() const
{
	return (_path);
}

const	std::string& Request::getQuery() const
{
	return (_query);
}

const	std::map<std::string, std::string>&	Request::getQueryEntries() const
{
	return (_query_entries);
}

const	std::string& Request::getQueryEntry(const std::string& key) const
{
	static const std::string empty;
	std::map<std::string, std::string>::const_iterator it = _query_entries.find(key);
	if (it != _query_entries.end())
		return (it->second);
	return (empty);
}

const	std::string& Request::getHttpVersion() const
{
	return (_http_version);
}

const	std::string& Request::getHeader(const std::string& key) const
{
	static const std::string empty;
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
		return (it->second);
	return (empty);
}

const	std::map<std::string, std::string>&	Request::getHeaders() const
{
	return (_headers);
}

const	std::string& Request::getContentType() const
{
	return (_content_type);
}

const	std::string& Request::getConnection() const
{
	return (_connection);
}

const	std::string& Request::getSessionID() const
{
	return (_session_id);
}

const	std::string& Request::getBody() const
{
	// /* debug */std::cout << "get body" << std::endl;
	return (_body);
}

// const 	std::map<std::string, std::string>&	Request::getCookies() const
// {
// 	return (_cookies);
// }

const	ParserState& Request::getState() const
{
	return (_state);
}

bool	Request::hasBody()
{
	return (!_body.empty());
}

HttpStatus	Request::getStatus() const
{
	return (_status);
}
	
bool 	Request::hasSessionId() const
{
	return (!_session_id.empty());
}

bool	Request::isParseComplete()
{
	return (_state == PARSE_COMPLETE);
}

void	Request::parseByState(size_t limit)
{
	// /* debug */std::cout << _parsed_pos << std::endl;
	// /* debug */std::cout << _raw.size() << std::endl;
	while (_parsed_pos < _raw.size())
	{
		switch (_state)
		{
			case PARSE_REQUEST_LINE:
				parseRequestLine(_raw, _parsed_pos);
				break;
			
			case PARSE_HEADERS:
				parseHeaders(_raw, _parsed_pos);
				decideBodyState();
				break;

			case PARSE_BODY:
			// /* debug */std::cout << "> state: PARSE_BODY" << std::endl;
			parseBody(_raw, _parsed_pos, limit);
				break;

			case PARSE_BODY_CONTENT_LENGTH:
			parseContentLengthBody(_raw, _parsed_pos, limit);
				break;
			
			case PARSE_BODY_CHUNKED_SIZE:
			case PARSE_BODY_CHUNKED_DATA:
			parseChunkedBody(_raw, _parsed_pos, limit);
			// /* debug */std::cout << "> state: PARSE_BODY_CHUNKED" << std::endl;
				break;

			case PARSE_COMPLETE:
			// /* debug */std::cout << "> state: PARSE_COMPLETE" << std::endl;
			case PARSE_ERROR:
				return;
		}
	}
}

void 	Request::parseRequestLine(const std::string& raw, size_t &pos)
{
	/* debug */std::cout << PINK << "> raw request: " << RESET << raw.substr(pos,100) << std::endl;
	size_t line_end = raw.find("\r\n", pos);
	if (line_end == std::string::npos)
	{
		/* debug */std::cout << PINK << "> REQ: no \\r\\n end found" << RESET << std::endl;
		_status = HTTP_BAD_REQUEST;
		_state = PARSE_ERROR;
		return;
	}	
	
	size_t method_end = raw.find(' ', pos);
	if (method_end == std::string::npos || method_end > line_end)
	{
		/* debug */std::cout << PINK << "> REQ: no method end found" << RESET << std::endl;
		_status = HTTP_BAD_REQUEST;
		_state = PARSE_ERROR;
		return;
	}

	size_t path_end = raw.find(' ', method_end + 1);
	if (path_end == std::string::npos || path_end > line_end)
	{
		/* debug */std::cout << PINK << "> REQ: no path end found" << RESET << std::endl;
		_status = HTTP_BAD_REQUEST;
		_state = PARSE_ERROR;
		return;
	}

	_method = std::string(&raw[pos], method_end - pos);
	/* debug */std::cout << PINK << "> REQ parsed method: " << RESET << _method << std::endl;
	
	std::string full_path = std::string(&raw[method_end + 1], path_end - method_end - 1);

	// query (starts from '?')
	// example.com/search?query=cat&sort=date&page=2
	size_t question_mark = full_path.find('?');
	if (question_mark != std::string::npos)
	{
		_path = std::string(&full_path[0], question_mark);
		_query = std::string(&full_path[question_mark + 1]);
		/* debug */std::cout << PINK << "> REQ parsed path: " << RESET << _path << std::endl;
		/* debug */std::cout << PINK << "> REQ parsed query: " << RESET << _query << std::endl;
		parseQuery(_query);
	}
	else
		_path = full_path;
	/* debug */std::cout << PINK << "> REQ parsed path: " << RESET << _path << std::endl;
	
	_http_version = std::string(&raw[path_end + 1], line_end - path_end - 1);
	/* debug */std::cout << PINK << "> REQ parsed version: " << RESET << _http_version << std::endl;

	validateRequestLine();
	if (_state != PARSE_ERROR)
	{
		pos = line_end + 2; // move past "\r\n"
		_state = PARSE_HEADERS;
	}
}

/* 
	query entries ('=' is key value, '&' is seperator)
	eg : "user=John+Doe&file=report%202025.pdf&flag"
	entry[1] : user=John+Doe
	entry[2] : file=report%202025.pdf
	entry[3] : flag (some query flags are just keys - no value)
*/
void	Request::parseQuery(const std::string& query)
{
	if (query.empty())
		return ;

	size_t start = 0;
	while (start < query.length())
	{
		// find next entry (seperated by &)
		size_t ampersand = query.find('&', start);
		size_t end = (ampersand == std::string::npos) ? query.length() : ampersand;

		// extract current entry (key=value)
		std::string entry = std::string(&query[start], end - start);

		// extract entry's key + value
		size_t equals = entry.find('=');
		if (equals != std::string::npos)
		{
			std::string key(&entry[0], equals);
			std::string value(&entry[equals + 1], entry.length() - equals);

			key = urlDecode(key);
			value = urlDecode(value);

			_query_entries[key] = value;
			/* debug */std::cout << PINK << "> REQ query entry: " << RESET << key << " = " << value << std::endl;
		}
		else
		{
			std::string key = urlDecode(entry);
			_query_entries[key] = "";
		}

		if (ampersand == std::string::npos)
			break;
		start = ampersand + 1;
	}
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
		// /* debug */std::cout << "\n> KEY:VALUE : " << key << " : " << value;

		_headers[key] = value;
		handleSpecialHeaders(key, value);
		pos = line_end + 2;
	}
	validateHeaders();
	pos = headers_end + 4; // move cursor to body_start, skipping header_end empty line
}

void	Request::parseCookies(const std::string& value)
{
	const std::string key = "session_id";
	size_t start = value.find(key);
	if (start == std::string::npos)
		return ;

	start += key.length() + 1;
	size_t end = value.find(';', start);
	/* debug */std::cout << "> session_id : " << _session_id << "\n" << std::endl;

	if (end == std::string::npos)
		_session_id = std::string(&value[start], value.size() - start);
	else
		_session_id = std::string(&value[start], end - start);

	_cookies["session_id"] = _session_id;

	/* debug */std::cout << "> session_id_2 : " << _session_id << "\n" << std::endl;
}

void	Request::parseBody(const std::string& raw, size_t &pos, size_t limit)
{
	// body starts after \r\n\r\n (header_end + empty line)
	if (pos >= raw.length())
		return; // no body data yet
	if (_isChunked)
	{
		_state = PARSE_BODY_CHUNKED_SIZE;
		parseChunkedBody(raw, pos, limit);
	}
	else if (_content_length > 0)
	{
		// /* debug */std::cout << "> check length > 0" << std::endl;
		_state = PARSE_BODY_CONTENT_LENGTH;
		parseContentLengthBody(raw, pos, limit);
	}
	// else if (_content_length == 0)
	// 	_state = PARSE_COMPLETE;
}

void	Request::parseChunkedBody(const std::string& raw, size_t &pos, size_t limit)
{
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
			_current_chunk_size = std::strtoul(hex.c_str(), NULL, 16);
			pos = line_end + 2;

			// check for last chunk
			if (_current_chunk_size == 0)
			{
				size_t trailer_end = raw.find("\r\n\r\n", pos);
				if (trailer_end != std::string::npos)
					pos = trailer_end + 4;
				// /* debug */std::cout << "> last chunk" <<std::endl;
				_state = PARSE_COMPLETE;
				return;
			}

			// move on to reading data
			// pos = line_end + 2;
			_state = PARSE_BODY_CHUNKED_DATA;
			// /* debug */std::cout << "> keep reading data" <<std::endl;
		}

		// read chunk data
		if (_state == PARSE_BODY_CHUNKED_DATA)
		{
			size_t available = raw.length() - pos;
			if (available < _current_chunk_size + 2) // \r\n
				return;
			_body.append(raw, pos, _current_chunk_size);

			// check size limit
			if (_body.size() > limit)
			{
				_status = HTTP_PAYLOAD_TOO_LARGE;
				_state = PARSE_ERROR;
				return ;
			}
			pos += _current_chunk_size + 2;
			_state = PARSE_BODY_CHUNKED_SIZE;
		}
	}
}

void	Request::parseContentLengthBody(const std::string& raw, size_t &pos, size_t limit)
{
	// calculate how much data is available (from buffered _raw)
	// /* debug */std::cout << "> parse content body" << std::endl;
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

	if (_body.size() > limit)
	{
		_status = HTTP_PAYLOAD_TOO_LARGE;
		_state = PARSE_ERROR;
		return ;
	}
	// /* debug */std::cout << "> " << _body << std::endl;
	pos += body_remaining;
	_state = PARSE_COMPLETE;
}

// void	Request::parseMultipart(const std::string& raw, size_t &pos, size_t limit)
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
	{
		_status = HTTP_BAD_REQUEST;
		_state = PARSE_ERROR;
		return;
	}
}

void	Request::validateHeaders()
{
	// missing host (typically absent with HTTP/1.0)
	if (_headers.find("host") == _headers.end() || _headers.at("host").empty())
	{
		/* debug */std::cout << PINK << "> REQ: invalid host header" << RESET << std::endl;
		_status = HTTP_BAD_REQUEST;
		_state = PARSE_ERROR;
	}
	// Chunked + Content-Length together > 400 error
	if (_isChunked && _content_length != 0)
	{
		_status = HTTP_BAD_REQUEST;
		_state = PARSE_ERROR;
		return;
	}
	// no content length, no chunked > 411 error
	if (_method == "POST" && !_isChunked && _headers.find("content-length") == _headers.end())
	{
		_status = HTTP_LENGTH_REQUIRED;
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
	else if (key == "content-type")
		_content_type = value;
}

void	Request::decideBodyState()
{
	if (_isChunked || _content_length > 0)
		_state = PARSE_BODY;
	else
		_state = PARSE_COMPLETE;
}

void	Request::clear()
{
	_raw.clear();
	_method.clear();
	_path.clear();
	_query.clear();
	_query_entries.clear();
	_http_version.clear();
	_headers.clear();
	_content_length = 0;
	_content_type.clear();
	_body.clear();
	_cookies.clear();
	_session_id.clear();
	_connection = "keep-alive";
	_state = PARSE_REQUEST_LINE;
	_parsed_pos = 0;
	_status = HTTP_OK;
	_isChunked = false;
	_current_chunk_size = 0;
}
