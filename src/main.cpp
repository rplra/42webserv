#include "Webserv.hpp"
#include "ConfigParse.hpp"

int g_signal;
void testConfigParser(const std::vector<Server>&servers);

/* 
    HTTP Server 
    - a computer program that serves webpages to clients over the HTTP protocol     

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

    GET method - headers + blank line
    POST method - headers + blank line + body
*/
int main(int ac, char **av)
{
	try
	{
	checkArgument(ac);
	std::string filename = av[1];

    // signal handling for shutting down
    g_signal = 1;
    signal(SIGINT, handleSignal);

	// create config + parser > parse the file
    Config          config;
	ConfigParser    parser(config);
    parser.parseConfig(av);
	// // print parsed servers
	// const std::vector<Server>& servers = config.getServers();
	// testConfigParser(servers); // tested with basic.conf

	// TO DO : ctrl+c to shutdown server? or should we enter "exit" to gracefully shut down the server?
	std::cout << BLUE << ">> Server running ... (Ctrl+C to stop)" << std::endl;

	// run webserver
	ServerManager webserv(config);
	webserv.run();

	std::cout << BLUE << ">> Webserv successfully shut down." << std::endl;

	}
	catch(const std::exception& e)
	{
		std::cerr << RED << "Exception: " << e.what() << RESET << std::endl;
		return (1);
	}

    return (0);
}



void testConfigParser(const std::vector<Server>&servers)
{
	std::cout << PURPLE << "Parsed " << servers.size() << " server/s: " << RESET << std::endl;

	for (size_t i = 0; i < servers.size(); ++i)
    {
        const Server& srv = servers[i];
        std::cout << ORANGE << "Server " << i << ":" << RESET << std::endl;
        std::cout << "  Host                : " << srv.getHost() << std::endl;
        std::cout << "  Port                : " << srv.getPort() << std::endl;
        std::cout << "  Root                : " << srv.getRoot() << std::endl;
        std::cout << "  Index               : " << srv.getIndex() << std::endl;
        std::cout << "  Autoindex           : " << (srv.getAutoindex() ? "on" : "off") << std::endl;
        std::cout << "  Client max body size: " << srv.getClientMaxBodySize() << std::endl;

        const std::vector<Location>& locs = srv.getLocations();
        for (size_t j = 0; j < locs.size(); ++j)
        {
            const Location& loc = locs[j];
            std::cout << YELLOW << "  Location " << j <<  RESET << std::endl;
            std::cout << "    Path           : " << loc._path << std::endl;
            std::cout << "    Root           : " << loc._root << std::endl;
            std::cout << "    Index          : " << loc._index << std::endl;
            std::cout << "    Autoindex      : " << (loc._autoindex ? "on" : "off") << std::endl;
            std::cout << "    Allowed methods: ";
            for (std::vector<std::string>::const_iterator it = loc._allowed_methods.begin(); it != loc._allowed_methods.end(); ++it)
                std::cout << *it << " ";
            std::cout << std::endl;
        }
    }
}