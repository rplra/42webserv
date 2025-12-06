#include "Client.hpp"

Client::Client(int clientSocket, const Server* server)
:
	_clientSocket(clientSocket),
	_server(server),
	_request(),
	_response(NULL),
	_bytesSent(0),
	_hasResponse(false)
{};

int	Client::getFd() const
{
	return (_clientSocket);
}

const Server*	Client::getServer() const
{
	return (_server);
}

Request& Client::getRequest()
{
	return (_request);
}

Response* Client::getResponse()
{
	return (_response);
}

bool Client::responseReady() const
{
	return (_hasResponse);
}

void Client::buildResponse()
{
	const Location* location = _server->bestMatchingLocation(_request.getPath());
	
	// clean up old response if exists
	if (_response)
	{
		delete _response;
		_response = NULL;
	}

	// 1. determine initial status from request parsing
	HttpStatus status = HTTP_OK;
	if (_request.getState() == PARSE_ERROR)
	{
		// map request parse state to HTTP status
		// may add method in Request to get error status
		status = HTTP_BAD_REQUEST;
	}
	
	// 2. create response object
	_response = new Response(&_request, *_server, status);

	// 3. handle parse errors
	if (status != HTTP_OK)
	{
		_response->setType(ERROR);
		_response->buildResponse();
		_hasResponse = true;
		_bytesSent = 0;
		return ;
	}

	/*********************************  TO REPLACE BY CONFIG (ROUTING) *********************************/
	// 4. check if method is allowed (if location sepcifies allows methods)
	if (location && !location->_allowed_methods.empty())
	{
		bool methodAllowed = false;
		for (size_t i = 0; i < location->_allowed_methods.size(); ++i)
		{
			if (location->_allowed_methods[i] == _request.getMethod())
			{
				methodAllowed = true;
				break;
			}
		}
		
		if (!methodAllowed)
		{
			_response->setType(ERROR);
			_response->setError(HTTP_METHOD_NOT_ALLOWED); // TO DO
			_response->buildResponse();
			_hasResponse = true;
			_bytesSent = 0;
			return;
		}
	}

	// 5. check for redirect
	if (location && !location->_redirect.empty())
	{
		// Get first redirect (assuming single redirect per location)
		std::map<int, std::string>::const_iterator it = location->_redirect.begin();
		if (it != location->_redirect.end())
		{
			_response->setType(REDIRECT);
			_response->redirect_path = it->second;
			_response->buildResponse();
			_hasResponse = true;
			_bytesSent = 0;
			return;
		}
	}

	// 6. build file path for static content
	// Start with root (location root overrides server root)
	std::string root = _server->getRoot();
	if (location && !location->_root.empty())
		root = location->_root;

	// Get request path and remove location prefix if present
	std::string requestPath = _request.getPath();
	if (location && !location->_path.empty())
	{
		size_t locLen = location->_path.length();
		// Only remove prefix if request path starts with location path
		if (requestPath.compare(0, locLen, location->_path) == 0)
		{
			requestPath = requestPath.substr(locLen);
			// Ensure path starts with /
			if (requestPath.empty() || requestPath[0] != '/')
				requestPath = "/" + requestPath;
		}
	}
	// combine root + request 
	std::string fullPath = root + requestPath;
	/*********************************  TO REPLACE BY CONFIG (ROUTING) *********************************/

	// store in response ( LATER RESPONSE call Server->getFilePath())
	_response->file_path = fullPath;
	_response->setType(STATIC);
	
	// 7. build the response
	_response->buildResponse();
	_hasResponse = true;
	_bytesSent = 0;
}

bool Client::sendResponse()
{
	if (!_response)
		return true;
	// get raw response bytes
	const std::string& responseData = _response->getRawResponse();

	if (_bytesSent >= responseData.size())
		return true; // all data sent

	// send remaining data
	ssize_t bytes = send(_clientSocket,
						responseData.c_str() + _bytesSent,
						responseData.size() - _bytesSent, 0);

	if (bytes < 0)
	{
		std::cerr << RED << "send() error on fd " << _clientSocket << RESET << std::endl;
		return true; // signal to remove client
	}
	else if (bytes == 0)
	{
		std::cerr << RED << "Connection closed while sending on fd " << _clientSocket << RESET << std::endl;
		return true;
	}

	_bytesSent += bytes;

	// return true if all sent
	return (_bytesSent >= responseData.size());
}

void Client::reset()
{
	// clear cant clear an object, so need to clear manually (all vars within the class)
	_request.clear();
	
	// clean up old response
	if (_response)
	{
		delete _response;
		_response = NULL;
	}

	_bytesSent = 0;
	_hasResponse = false;
	
	// bytesSent = 0;	// reset send counter
	// _hasResponse = false;
}