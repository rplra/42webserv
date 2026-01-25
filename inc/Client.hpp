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

	void			setServer(const Server* server);
	void			setServerSocket(int fd);
	int				getServerSocket();

	bool			responseReady() const;
	void			markResponseReady();
	void			handleResponse();
	bool			sendResponse();
	bool			hasSendError();
	
	void			reset();

private:
	const Server*	_server;
	int				_clientSocket;
	int				_serverSocket;
	Request			_request;
	Response*		_response;
	size_t			_bytesSent;

	bool			_hasResponse;
	bool			_sendError;
};

#endif