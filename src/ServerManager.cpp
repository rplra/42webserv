#include "ServerManager.hpp"

ServerManager::ServerManager(const Config& config)
: 	_config(config),
	_serverSockets(),
	_socketToServer(),
	_clients(),
	_pollFds()
{};

void ServerManager::run()
{
	const std::vector<Server>& servers = _config.getServers();
    if (servers.empty())
        throw std::runtime_error(ERR_SERVERCONFIG);

	createAllListeningSockets();
	createPollFds();

	while (g_signal)
	{
		// a. wait for events
		int ready_count = poll(_pollFds.data(), _pollFds.size(), _pollTimeoutMs);
        /* debug */std::cout << GREEN << "Ready: " << RESET << ready_count << std::endl;
        if (ready_count < 0) {
            if (errno == EINTR)
                continue; // interrupted by signal
            else {
				cleanUp();
                throw std::runtime_error(ERR_POLL);
			}
		}

		// b. loop over all pollfds
		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			int fd = _pollFds[i].fd;
			// listen for incoming connections
			if (_pollFds[i].revents & POLLIN && isServerSocket(fd))
				acceptNewClient(fd);
			else if (_pollFds[i].revents & POLLIN && !isServerSocket(fd))
				handleEventRead(fd);
			if (_pollFds[i].revents & POLLOUT && _clients.find(fd) != _clients.end()
					&& _clients[fd]->responseReady())
				handleEventWrite(fd);
		}
	}
	// 6. cleanup
	cleanUp();
}

void ServerManager::createAllListeningSockets()
{
	const std::vector<Server>& servers = _config.getServers();

    for (size_t i = 0; i < servers.size(); ++i) 
	{
        int serverSocket = createListeningSocket(servers[i].getHost(), servers[i].getPort());
        if (serverSocket < 0) 
			throw std::runtime_error(ERR_CREATEALLSOCK + std::to_string(servers[i].getPort()));

        _serverSockets.push_back(serverSocket);
		// map serversocket to the server obj
		_socketToServer[serverSocket] = &servers[i];
    }
}

int ServerManager::createListeningSocket(std::string host, int port)
{
	// Attach the socket to the port 
    // struct addrinfo {
    //     int              ai_flags;       // Options for getaddrinfo (e.g., AI_PASSIVE)
    //     int              ai_family;      // Address family (AF_INET, AF_INET6, AF_UNSPEC) 
    //     int              ai_socktype;    // Socket type (SOCK_STREAM, SOCK_DGRAM)
    //     int              ai_protocol;    // Protocol (TCP = 6, UDP = 17, usually 0 for auto)
    //     socklen_t        ai_addrlen;     // Length of ai_addr
    //     struct sockaddr *ai_addr;        // Pointer to actual address (sockaddr_in)
    //     char            *ai_canonname;   // Canonical name for hostname (if requested)
    //     struct addrinfo *ai_next;        // Pointer to next addrinfo in linked list
    // };
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));

    hints.ai_flags = AI_PASSIVE;      // Only allow binding to local IP addresses
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP
    std::cout << GREEN << "Creating listening socket on " << port << RESET << std::endl;
    
	int status = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res);
    if ((status) != 0)
		throw std::runtime_error(ERR_GETADDRINFO + gai_strerror(status));

    // 1. Create socket 
    // AF_INET for IPv4, SOCK_STREAM for TCP
    // Server server; 
    int serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (serverSocket < 0)
        throw std::runtime_error(ERR_CREATESOCK);

    // 2. Set SO_REUSEADDR to allow quick reuse of the port
    // Set opt to 1 to enable the option
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. Bind the socket to the specified IP/port
    if(bind(serverSocket, res->ai_addr, res->ai_addrlen) < 0)
	{
		freeaddrinfo(res);
        close(serverSocket);
        throw std::runtime_error(ERR_BINDSOCK);
    }

    // 4. Listen for incoming connections
    if (listen(serverSocket, 5) < 0)
	{
		freeaddrinfo(res);
        close(serverSocket);
		throw std::runtime_error(ERR_LISTENSOCK);
    }

    // 5. Non-blocking mode for server socket
    // accept() uses serverSocket to wait for connection
    // If serverSocket is blocking, accept() will freeze the whole server
    // Retrieve current flag of the socket and 0 means does not change the flags
    int flags = fcntl(serverSocket, F_GETFL, 0);
    // preserve existing flags and add O_NONBLOCK flag 
    fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK);

    freeaddrinfo(res);
    return serverSocket;
}

void ServerManager::createPollFds()
{
	for (size_t i = 0; i < _serverSockets.size(); ++i)
	{
        pollfd server_fd;
        server_fd.fd = _serverSockets[i];
        server_fd.events = POLLIN; // reading data
        _pollFds.push_back(server_fd);
    }
}

void	ServerManager::addPollFd(int fd, short events)
{
    pollfd pfd;
    pfd.fd = fd;
    pfd.events = events; // reading (POLLIN) / writing (POLLOUT)
    _pollFds.push_back(pfd);
}

// whenever we close a client, we close(clientFd) > _clients.erase(clientFd) > removePollFd(clientFd)
void	ServerManager::removePollFd(int fd)
{
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (fd == _pollFds[i].fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break ;
		}
	}
}

void	ServerManager::acceptNewClient(int serverSocket)
{
	// 1. accept a connection first
	// accept() creates a new socket FD for client
	// each client get its own socket seperate from listening socket
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket < 0)
        return ;

	// 2. set Non-blocking mode for client socket immediately
    // recv() uses clientSocket to read data
    // If clientSocket is blocking, recv() will freeze the whole server
    // Retrieve current flags of the socket and set O_NONBLOCK
    int flags = fcntl(clientSocket, F_GETFL, 0);
    fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

	// 3. create and store client obj
	// get the server obj that this server socket belongs to
	const Server* serverPtr = _socketToServer[serverSocket];
	// map the client to its own socket and the server obj it connected to
    Client* client = new Client(clientSocket, serverPtr);
	// store the client into map (allow serverManager to find client quickly when socket has activity)
	_clients[clientSocket] = client;
    /* debug */std::cout << GREEN << "Mapped client fd: " << clientSocket << " to server fd: " << serverSocket << RESET << std::endl;

	//4. add to pollfd
	// create new pollfd struct for this client socket
	// events = POLLIN - we want to read from the client when it sends data
	// push client_fd into fds vector so poll() can start monitoring
	addPollFd(clientSocket, POLLIN);
	// from now on, client FD is mointored like all others
}

void	ServerManager::handleEventRead(int clientSocket)
{
	Client* client = _clients[clientSocket];
	if (!client)
		return ;
	
	char	buffer[BUFFER_SIZE];
	ssize_t	bytes = recv(clientSocket, buffer, sizeof(buffer), 0);

	if (bytes == 0)
	{
		std::cout << YELLOW << ERR_CLIENTDISCONECT << clientSocket << RESET << std::endl;
		removeClient(clientSocket);
		return ;
	}
	else if (bytes < 0)
	{
		std::cerr << RED << ERR_RECVFD << clientSocket << RESET << std::endl;
		removeClient(clientSocket);
		return ;
	}

	// parse the received data
	client->getRequest().handleRequest(buffer, bytes, client->getServer()->getClientMaxBodySize());
	// if parsing is complete or state is parse_error, generate response
	if (client->getRequest().isParseComplete() || client->getRequest().getState() == PARSE_ERROR)
	{
		client->buildResponse();			// build response (routing handled by config > file_path)
		enableWriteEvent(clientSocket);		// enable POLLOUT so we can send the data
	}
}

void	ServerManager::handleEventWrite(int clientSocket)
{
	Client* client = _clients[clientSocket];
	if (!client)
		return;

	bool sendComplete = client->sendResponse();
	if (sendComplete)
	{
		disableWriteEvent(clientSocket);
		if (isKeepAlive(client))	// keep connection alive for next request 
			client->reset();		// clear request/response data
		else
			removeClient(clientSocket);
	}
}

void	ServerManager::enableWriteEvent(int clientSocket)
{
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (_pollFds[i].fd == clientSocket)
		{
			_pollFds[i].events |= POLLOUT;
			break ;
		}
	}
}

void	ServerManager::disableWriteEvent(int clientSocket)
{
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (_pollFds[i].fd == clientSocket)
		{
			_pollFds[i].events &= ~POLLOUT;
			break ;
		}
	}
}

bool	ServerManager::isServerSocket(int fd)
{
	return (_socketToServer.find(fd) != _socketToServer.end());
}

bool	ServerManager::isKeepAlive(Client* client)
{
	// check connection header in request
	const std::string& connection = client->getRequest().getHeader("Connection");

	// HTTP/1.1 defaults to keep-aloive unless explicitly closed
	if (client->getRequest().getHttpVersion() == "HTTP/1.1")
		return (connection != "close");
	else
		return (connection == "keep-alive" || connection == "Keep-Alive");
}

/* 
	1. find clientSocket/Fd in vec _pollFd
	2. close clientFd
	3. delete client obj from _clients map
	4. remove pollfd entry
	USE: 	handleEventRead > when client disconnect
			handleEventWrite > response done 
*/
void	ServerManager::removeClient(int clientSocket)
{
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (_pollFds[i].fd == clientSocket)
		{
			close(clientSocket);

			std::map<int, Client*>::iterator it = _clients.find(clientSocket);
			if (it != _clients.end())
			{
				delete it->second;
				_clients.erase(it);
			}
			_pollFds.erase(_pollFds.begin() + i);
			return;
		}
	}
}

/* 
	Close all the client and server sockets i.e. all fds in _clients and _serverSockets
	Delete all client objects in _clients map since we used new to create them
	Empty pollFds vector
*/
void	ServerManager::cleanUp()
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second) // can remove if didnt assign NULL
		{
			close(it->second->getFd());	// close client socket;
			delete (it->second);		// free client object;
		}
	}
	_clients.clear(); // remove all entries

	// /* debug */std::cout << RED << "closing all fd..." << RESET << std::endl;
	for (size_t i = 0; i < _serverSockets.size(); i++)
		close(_serverSockets[i]);
	_serverSockets.clear();
	_pollFds.clear();
}
