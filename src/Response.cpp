// #include "Response.hpp"
// #include "Config.hpp"

#include "Webserv.hpp"

Response::Response(const Request* request, const Server& server, HttpStatus status) 
:
	_http_version("HTTP/1.1"),
	_status_code(status),
	_reason_phrase(),
	_headers(),
	_body(),
	_content_type(),
	_raw_response(),
	_isBuilt(false),
	_request(request),
	_server(server)
{
	setStatus(status);
}

// 1. what type of response is this? - error, static, index, cgi, redirect?
// 2. where does the content come from?
// 	- generated in memmory; error pages, html
// 	- read from disk (static files - img, html, css)
// 	- executed and captured - CGI
// 3. what metadata must accompany it? content type, content length, special headers (location for redirects)
std::string	Response::buildResponse()
{
	// if (_isBuilt)
	// 	return getRawResponse(); // return cached response
	
	/* debug */std::cout << PINK << "> building response" << RESET << std::endl;
	switch(_type)
	{
		case (REDIRECT):	buildRedirect(); break;
		case (STATIC):		buildStatic(); break;
		case (AUTOINDEX):	buildAutoIndex(); break;
		case (CGI):			buildCgi(); break;
		case (ERROR):		buildError(); break;
	}

	_isBuilt = true;
	// return (getRawResponse());
	return (_raw_response);
}

bool	Response::isResponseReady() const
{
	return (_isBuilt);
}

void Response::setType(ResponseType type)
{
	_type = type;
}

void	Response::setError(HttpStatus code)
{
	_type = ERROR;
	setStatus(code);
	buildError();
}

void	Response::setStatus(HttpStatus status)
{
	_status_code = status;
	_http_version = "HTTP/1.1";

	switch (_status_code)
	{
		case (HTTP_OK):						_reason_phrase = "Ok"; break;
		case (HTTP_FOUND):					_reason_phrase = "Found"; break;
		case (HTTP_BAD_REQUEST):			_reason_phrase = "Bad Request"; break;
		case (HTTP_FORBIDDEN):				_reason_phrase = "Forbidden"; break;
		case (HTTP_NOT_FOUND):				_reason_phrase = "Not Found"; break;
		case (HTTP_METHOD_NOT_ALLOWED):		_reason_phrase = "Method Not Allowed"; break;
		case (HTTP_LENGTH_REQUIRED):		_reason_phrase = "Length Required"; break;
		case (HTTP_PAYLOAD_TOO_LARGE):		_reason_phrase = "Payload Too Large"; break;
		case (HTTP_INTERNAL_SERVER_ERROR):	_reason_phrase = "Internal Server Error"; break;
		case (HTTP_NOT_IMPLEMENTED):		_reason_phrase = "Not Implemented"; break;
		case (HTTP_BAD_GATEWAY):			_reason_phrase = "Bad Gateway"; break;
		case (HTTP_SERVICE_UNAVAILABLE):	_reason_phrase = "Service Unavailable"; break;
	}
}

void	Response::setHeader(const std::string& key, const std::string& value)
{
	_headers[key] = value;
}

void	Response::setHeaders()
{
	_headers["Date"] = getDate();
	_headers["Server"] = "Webserv/1.0";
	_headers["Content-Length"] = std::to_string(_body.size());
	
	if (_request)
		_headers["Connection"] = _request->getConnection();
	else
		_headers["Connection"] = "close";
	
	if (!_content_type.empty())
		_headers["Content-Type"] = _content_type;
	if (_request->hasSessionId())
		_headers["Set-Cookie"] = "session_id=" + _request->getSessionID() + "; Path=/";
	if (_type == STATIC)
		_headers["Last-Modified"] = getLastModified();
}

void	Response::setBody(const std::string& body)
{
	_body = body;
}

/* 
	1. set _status to 301 or 302
	2. set Location header from config
	3. optional - generate small HTML body saying "Moved Permanently"
*/
void	Response::buildRedirect()
{
	/* debug */std::cout << PINK << "> build redirect() for path: " << RESET << _request->getPath() << "'" << std::endl;

	const Location* loc = _server.getMatchingLocation(_request->getPath());
	if (!loc)
	{
		/* debug */std::cerr << RED << "> no matching loc " << RESET << std::endl;
		setError(HTTP_INTERNAL_SERVER_ERROR);
		return ;
	}
	/* debug */std::cout << PINK << "> matching loc path: " << RESET << loc->_path << "'" << std::endl;
	int				code = HTTP_FOUND;
	std::string		url;

	std::map<int, std::string>::const_iterator it = loc->_redirect.find(code);
	if (it != loc->_redirect.end())
		url = it->second;
	else
	{
		setError(HTTP_INTERNAL_SERVER_ERROR);
		return ;
	}

	setStatus(static_cast<HttpStatus>(code));
	setHeader("Location", url);
	setBody("");
	setHeaders();
}

/* 
	1. open file (file_path) > read
	2. set content type (call getmimetype(file_path))
	3. set content length (file_size)
	4. set http status (200 success / 404 not found)
*/
void	Response::buildStatic()
{
	std::string full_path = _server.getFullPath(*_request);
	std::cout << PINK << "> build static for path: " << RESET << full_path << "'" << std::endl;

	struct stat file_stat;
	// file exist?
	if (stat(full_path.c_str(), &file_stat) != 0)
	{
		std::cout << PINK << "> build static: file not found" << RESET << std::endl;
		return (setError(HTTP_NOT_FOUND));
	}
	// is directory?
	if (S_ISDIR(file_stat.st_mode))
	{
		/* debug */std::cout << PINK << "> build static: is directory" << RESET << std::endl;
		handleDirectory(full_path);
		return;
	}
	// regular file?
	if (!S_ISREG(file_stat.st_mode))
		return (setError(HTTP_FORBIDDEN));

	serveFile(full_path, HTTP_OK);
}

/* 
	if request path points to a directory and autoindex is on
	1. generate an HTML listing of directory contents
	2. set _body to the HTML string
	3. set headers; content-type: text/html, content-length
*/
void	Response::buildAutoIndex()
{
	std::string file_path = _server.getFullPath(*_request);

	DIR *dir = opendir(file_path.c_str());
	if (!dir)
	{
		setError(HTTP_FORBIDDEN); 
		return;
	}
	closedir(dir);

	_status_code = HTTP_OK;
	setBody(generateAutoIndexBody(file_path));
	setHeader("Content-Type", "text/html");
	setHeaders();
}

/* 
	1. execute cgi script w/ request environment > capture output
	2. parse cgi headers (especially content-type)
	3. set _body to the remaining content
	4. set other HTTP headers from CGI output
	5. if CGI didnt provide content-type, call getMimeType
*/
void	Response::buildCgi()
{
	/* debug */std::cout << PINK << "> building cgi" << RESET << std::endl;
}

/* 
	1. check if there's custom error config-ed for the status
	2. if yes > serve (read file > detect mime > set headers)
	3. if no > generate simple HTML fallback page with status code / msg
*/
void	Response::buildError()
{
	/* debug */std::cout << PINK << "> building error" << RESET << std::endl;
	// std::map<HttpStatus, std::string>::const_iterator it = _server.getErrorPagePath(_status_code).find(_status_code);
	// if (it != _server.getErrorPagePath().end())
	std::string error_file = _server.getErrorPagePath(_status_code);
	if (!error_file.empty())
	{
		// resolve relative to server root
		std::string full_path = _server.getRoot() + error_file;

		struct stat st;
		if (stat(full_path.c_str(), &st) == 0 && S_ISREG(st.st_mode))
		{
			// file exist > serve
			/* debug */std::cout << PINK << "> serving error page" << RESET << std::endl;
			serveFile(full_path, _status_code);
			return;
		}
	}
	// else
	// 	generateErrorPage(_status_code);
}

std::string Response::getDate()
{
	char timestamp[128];
	std::time_t t = std::time(NULL);
	std::tm* gmt = std::gmtime(&t);
	std::strftime(timestamp, sizeof(timestamp), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	return (timestamp);
}

/* 
	- struct stat <sys/stat.h> holds info about a file
		// contains metadata; mode (type + perm), size, mtime (last mod),
						atime (lass access), ctime (last status change), etc
	- stat(file path, pointer to struct stat); get metadata w/out opening file
		// 0 (success), -1 (fail; file x exist, x perm, etc)
	- sm_mtime (modification time); time_t st_mtime (since Unix)
	- S_ISREG; returns true if file is regular file
	- file_stat.st_mode contains file type + permissions encoded in bit flags
	- std::tm; struct of time in sec, min, hr, day, month, year, etc
	
	1. check if file is not empty, file exists, file is a regular file
	2. get last modified time via st_mtime > format > return
	3. return empty is no last modified since its optional
*/
std::string	Response::getLastModified()
{
	std::string file_path = _server.getFullPath(*_request);

	struct stat file_stat;
	if (!file_path.empty() && 
		stat(file_path.c_str(), &file_stat) == 0 &&
		S_ISREG(file_stat.st_mode))
	{
		std::tm* gmt = std::gmtime(&file_stat.st_mtime);
		char last_modified[128];	
		std::strftime(last_modified, sizeof(last_modified), "%a, %d %b %Y %H:%M:%S GMT", gmt);
		return (last_modified);
	}
	return ("");
}

// file_path is from config; server's root + http's request path
std::string Response::getMimeType(const std::string& file_path)
{
	// extract file's extension
	size_t dot = file_path.find_last_of('.');
	if (dot == std::string::npos)
		return ("application/octet-stream");
	
	std::string ext = std::string(&file_path[dot], file_path.length() - dot);
	if (ext == ".html" || ext == ".htm")	return "text/html";
	if (ext == ".css")						return "text/css";
	if (ext == ".txt")						return "text/plain";
	if (ext == ".jpg" || ext == ".jpeg")	return "image/jpeg";
	if (ext == ".png")						return "image/png";
	if (ext == ".gif")						return "image/gif";
	if (ext == ".js")						return "application/javascript";
	if (ext == ".json")						return "application/json";

	return ("application/octet-stream");
}

std::string	Response::getRawResponse()
{
	_raw_response.clear();
	
	// build status line
	/* debug */std::cout << ORANGE << "> getting raw response" << RESET << std::endl;
	_raw_response += _http_version + " "
					+ std::to_string(_status_code) + " " 
					+ _reason_phrase + "\r\n";
	// /* debug */std::cout << ORANGE << "> status line: \n" << RESET << _raw_response << std::endl;
	
	// build headers
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		_raw_response += it->first + ": " + it->second + "\r\n";
	// /* debug */std::cout << ORANGE << "> status line + header: \n" << RESET << _raw_response << std::endl;
	
	// empty line
	_raw_response += "\r\n";

	// build body
	_raw_response += _body;
	/* debug */std::cout << ORANGE << "> full response: \n" << RESET << _raw_response << std::endl;
	return (_raw_response);
}

std::string Response::getFileBody(const std::string& path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open())
		return (setError(HTTP_INTERNAL_SERVER_ERROR), "");

	// read entire file and copy into a string
	// std::istreambuf_iterator<char>(file) ; pointer to first byte in file (begin)
	// std::istreambuf_iterator<char>() ; end of stream / EOF (end)
	// reads every byte, append each byte into string, ends when EOF
	std::string body((std::istreambuf_iterator<char>(file)),
						std::istreambuf_iterator<char>());
	file.close();
	return (body);
}

void Response::handleDirectory(const std::string& dir_path)
{
	std::string request_path = _request->getPath();

	// redirect if missing trailing slash
	if (!request_path.empty() && request_path.back() != '/')
	{
		std::cout << PINK << "> handle directory: missing trailing slash, redirecting..." << RESET << std::endl;
		_type = REDIRECT;
		setHeader("Location", request_path + "/");
		setBody("");
		setHeaders();
		setStatus(HTTP_FOUND);
		return;
	}
	
	// check if index.html exist inside directory
	std::string index_path = dir_path + "/index.html";
	struct stat index_stat;
	if (stat(index_path.c_str(), &index_stat) == 0 && S_ISREG(index_stat.st_mode))
	{
		// serve index file like normal static file
		serveFile(index_path, _status_code);
		return;
	}

	// else, generate index file
	const Location* location = _server.getMatchingLocation(request_path);
	bool autoindex = _server.getAutoindex();	// server default
	if (location)
		autoindex = location->_autoindex;		// location override
	if (autoindex)
	{
		/* debug */std::cout << PINK << "> handle directory: autoindex ON" << RESET << std::endl;
		_type = AUTOINDEX;
		buildAutoIndex();
		return;
	}
	
	// no index.html && autoindex > forbidden
	setError(HTTP_FORBIDDEN);
}

void Response::serveFile(const std::string& file_path, HttpStatus status)
{
	/* debug */std::cout << PINK << "> RESPONSE: serve file: " << RESET << file_path << std::endl; 
	setStatus(status);
	setBody(getFileBody(file_path));
	setHeader("Content-Type", getMimeType(file_path));
	/* debug */std::cout << PINK << "> RESPONSE: mime type: " << RESET << getMimeType(file_path) << std::endl; 
	setHeaders();
}

// basic for now, REVISE later
// nginx - file size, last mod, proper table, sorting, parent dir
std::string	Response::generateAutoIndexBody(const std::string& file_path)
{
	std::string html;

	DIR *dir = opendir(file_path.c_str());
	if (!dir)
		return (setError(HTTP_FORBIDDEN), "");

	html += "<!DOCTYPE html>\n<html>\n  <body>\n";
	html +=	"   <h1>Index of " + _request->getPath() + "</h1>\n";
	html += "   <ul>\n";

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name == "." || name == "..")
			continue;
		// optionally add '/' for directories
		std::string full_path = file_path + "/" + name;
		struct stat st;
		if (stat(full_path.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
			name += "/";
		// make url relative to request path, not filesystem path
		html += "      <li><a href=\"" + _request->getPath() + name + "\">" + name + "</a></li>\n";
	}
	closedir(dir);
	html += "   </ul>\n  </body>\n</html>\n";
	return (html);
}

// std::string Response::generateErrorPage(HttpStatus status)
// {
// 	std::string error_code = std::to_string(status);

// 	std::string html =
// 	"<!DOCTYPE html><html><head>"
//     "<meta charset=\"UTF-8\">"
//     "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
//     "<title>404 - Not Found</title>"
//     "</head><body>"
//     "<h1>" + error_code + " - Unknown Error</h1>"
//     "<p>An unidentified error occured.</p>"
//     "</body></html>";

// 	return (html);
// }
