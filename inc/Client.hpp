#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include "Webserv.hpp"

class Client
{
public:
	Client(int clientSocket, const Server* server);
	~Client();

	int				getFd() const;
	const Server*	getServer() const;
	Request&		getRequest();
	Response*		getResponse();

	bool			responseReady() const;
	void			markResponseReady();
	void			buildResponse();
	bool			sendResponse();
	
	void			reset();

private:
	int				_clientSocket;
	const Server*	_server;
	Request			_request;
	Response*		_response;
	size_t			_bytesSent;

	bool			_hasResponse;
};

#endif