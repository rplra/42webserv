#include "Webserv.hpp"
#include "ConfigParse.hpp"

int g_signal;

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

    Config          config;
	ConfigParser    parser(config);
    parser.parseConfig(av);

	// TO DO : ctrl+c to shutdown server? or should we enter "exit" to gracefully shut down the server?
	std::cout << BLUE << ">> Server running ... (Ctrl+C to stop)" << std::endl;

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
