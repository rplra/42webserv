#ifndef __SERVERMANAGER_HPP__
#define __SERVERMANAGER_HPP__

#include "Webserv.hpp"

class Client;
class Config;

class ServerManager
{
public:
	ServerManager(const Config& config);
	~ServerManager() {};

	void	run();

private:
	const Config&								_config;
	std::vector<int>							_serverSockets;
	std::map<int, std::vector<const Server*> >	_socketToServer;
	std::map<int, Client*>						_clients;
	std::vector<pollfd>							_pollFds;
	static const int							_pollTimeoutMs = 5000;

	int				createListeningSocket(std::string host, int port);
	void			createAllListeningSockets();
	void			createPollFds();
	void			addPollFd(int fd, short events);
	void			removePollFd(int fd);

	void			acceptNewClient(int serverSocket);
	void			removeClient(int clientSocket);

	void			selectServer(Client* client);
	void			handleEventRead(int clientSocket);
	void			handleEventWrite(int clientSocket);
	void			enableWriteEvent(int clientSocket);
	void			disableWriteEvent(int clientSocket);

	bool			isServerSocket(int fd);
	bool			isKeepAlive(Client* client);
	void			cleanUp();
};

#endif

/* 
    Flow of a server-side socket programming 
    1. socket() - create a socket
    2. bind() - bind the socket to an IP/port
    3. listen() - limits how many connections can wait before being accepted
    4. accept() - accept a connection
    5. recv() - receive data from a connection
    6. close() - close the connection

    1. set up server socket and get the port number from config file 
    2. receive request from client and store in a struct
    3. process the request and generate a response
*/

/* 
	config :: _port_map: maps port number → servers
	svrmgr :: _socketToServer: maps socket file descriptor → servers
			: virtual hosting; multiple servers can share same socket

	std::map<int, std::vector<const Server*> >	_socketToServer;
	using Server* (pointer)
	- _config owns the Server objs
	- _socketToServer just needs to reference them, not copy them
	- avoid copying Server obj itself into the map
		- takes up more memory
		- changes to original server would not reflect to copy
		- multiple client to same server but different copies is wrong
	- _listenServer points to existing Server obj, no copies / duplication 
	- clients sotre a pointer to the server they belong to

	_config will take reference from the config obj itself, 
	_serversockets are created based config's server in createListeningSocket, 
	_socketToServer is mapped to server obj when createAllListeningSocket, 
	_clients are created when we need to accept a connection (checked during run if its listenFd), 
	_pollfds are collective addition of listeningfd (serverSocket) and connectionfd (clientSocket)
*/