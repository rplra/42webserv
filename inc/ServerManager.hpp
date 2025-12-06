#ifndef __SERVERMANAGER_HPP__
#define __SERVERMANAGER_HPP__

#include "Webserv.hpp"

class Client;
class ServerManager
{
public:
	ServerManager(const Config& config);
	~ServerManager() {};

	void	run();

private:
	const Config&					_config;
	std::vector<int>				_serverSockets;
	std::map<int, const Server*>	_listentoServer;
	std::map<int, Client*>			_clients;
	std::vector<pollfd>				_pollFds;

	int		createListeningSocket(std::string host, int port); 	// done
	void	createAllListeningSockets();						// done
	void	createPollFds();									// done
	void	addPollFd(int fd, short events);					// done
	void	removePollFd(int fd);								// done

	void	acceptNewClient(int serverSocket);					// done
	void	removeClient(int clientSocket);						// closeClient  ; done

	void	handleEventRead(int clientSocket);					// wip
	void	handleEventWrite(int clientSocket);					// wip
	void	enableWriteEvent(int clientSocket);
	void	disableWriteEvent(int clientSocket);

	bool	isListenFd(int fd);
	bool	isKeepAlive(Client* client);
	void	cleanUp();

};

#endif