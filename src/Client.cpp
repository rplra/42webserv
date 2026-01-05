#include "Client.hpp"

Client::Client(int clientSocket, const Server* server)
:
	_server(server),
	_clientSocket(clientSocket),
	_serverSocket(-1),
	_request(),
	_response(NULL),
	_bytesSent(0),
	_hasResponse(false),
	_sendError(false)
{}

Client::~Client()
{
	delete _response;
}

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

void Client::setServer(const Server* server)
{
	_server = server;
}

void Client::setServerSocket(int fd)
{
	_serverSocket = fd;
}

int	Client::getServerSocket()
{
	return (_serverSocket);
}

bool Client::responseReady() const
{
	return (_hasResponse);
}

void Client::markResponseReady()
{
	_hasResponse = true;
	_bytesSent = 0;
}

void Client::buildResponse()
{
	const Location* location = _server->getMatchingLocation(_request.getPath());
	
	// clean up old response if exists
	if (_response)
	{
		delete _response;
		_response = NULL;
	}

	HttpStatus status = _request.getStatus();
	_response = new Response(&_request, *_server, status);

	// handle parse errors
	if (status != HTTP_OK)
	{
		_response->setType(ERROR);
		_response->setError(status);
		_response->buildResponse();
		markResponseReady();
		return ;
	}

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
			_response->setError(HTTP_METHOD_NOT_ALLOWED);
			_response->buildResponse();
			markResponseReady();
			return;
		}
	}

	// 5. check for redirect (config redirect)
	// if a redirect can be decided without touching the filesystem
	if (location && !location->_redirect.empty())
	{
		_response->setType(REDIRECT);
		_response->buildResponse();
		markResponseReady();
		return;
	}

	// 6. else, serve static content
	_response->setType(STATIC);

	// 7. POST or DELETE overrides static
	if (_request.getMethod() == "POST")
		_response->setType(POST_HANDLER);
	else if (_request.getMethod() == "DELETE")
		_response->setType(DELETE_HANDLER);

	// 8. CGI overrides
	if (location && location->_cgi.size() > 0)
		_response->setType(CGI);

	_response->buildResponse();
	markResponseReady();
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
		std::cerr << RED << ERR_SENDERROR << _clientSocket << RESET << std::endl;
		_sendError = true;
		return true;
	}
	else if (bytes == 0)
	{
		std::cerr << RED << ERR_SENDCONNCLOSED << _clientSocket << RESET << std::endl;
		_sendError = true;
		return true;
	}

	_bytesSent += bytes;
	// return true if all sent
	return (_bytesSent >= responseData.size());
}

bool Client::hasSendError()
{
	return (_sendError);
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

	_bytesSent = 0;		// reset send counter
	_hasResponse = false;
}